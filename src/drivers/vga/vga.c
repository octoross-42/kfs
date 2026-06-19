#include "vga.h"

// vga a un text buffer à l'adresse physique 0xB8000
// 	-> buffer = grille de 80x25 = 2000 cellules
// 	-> cellule = 16 bytes -> [octet 1: attribut -> [bg 4 bits, fg bits]] [octet 0: ascii] (little endian, mais on gere pas, le compilateur gere, visible depuis gdb)

static size_t			vga_row = 0;
static size_t			vga_column = 0;
static enum vga_color	vga_fg = VGA_COLOR_DARK_GREY;
static enum vga_color	vga_bg = VGA_COLOR_BLACK;

static uint16_t			alt_screen[VGA_WIDTH * VGA_HEIGHT];
static size_t			alt_row = 0;
static size_t			alt_column = 0;

static uint8_t			cursor_toggle = 1;
static uint32_t			cursor_tick = 0;

void	vga_change_fg(enum vga_color fg) { vga_fg = fg; }
void	vga_change_bg(enum vga_color bg) { vga_bg = bg; }

static void	apply_ansi_code(unsigned int code)
{
	static const enum vga_color	ansi_to_vga[8] =
	{
		0x0, // Black
        0x4, // Red
        0x2, // Green
        0x6, // Yellow/Brown
        0x1, // Blue
        0x5, // Magenta
        0x3, // Cyan
        0x7, // White
	};

	if (code == 0)
	{
		vga_change_fg(VGA_COLOR_DARK_GREY);
		return vga_change_bg(VGA_COLOR_BLACK);
	}
	if (code == 1)						// bold
		return vga_change_fg(vga_fg | 0b1000);	// bit 3 = bit d'intensité
	if ((code == 2) || (code == 22))	// dim
		return vga_change_fg(vga_fg & 0b0111);	// bit 3 = bit d'intensité
	if (code == 5)						// blink ?
		return vga_change_bg(vga_bg | 0b1000);	// bit 7 = bit de blink (entry = bg (4bits) + fg (4bits) + c (8 bits)), pour styliser -> 8 bits
	if (code == 7)						// reverse
	{
		enum vga_color bg = vga_bg;
		vga_change_bg(vga_fg);
		return vga_change_fg(bg);
	}
	if ((30 <= code) && (code <= 37))
		return vga_change_fg(ansi_to_vga[code - 30]);
	if ((40 <= code) && code <= 47)
		return vga_change_bg(ansi_to_vga[code - 40]);
	if ((90 <= code) && (code <= 97))
		return vga_change_fg(ansi_to_vga[code - 90] + 8);
	if ((100 <= code) && code <= 107)
		return vga_change_bg(ansi_to_vga[code - 100] + 8);
	return ;
}

static unsigned int	apply_ansi(char *str)
{
	unsigned int i = 0;
	unsigned int code = 0;
	while (str[i])
	{
		if (('0' <= str[i]) && (str[i] <= '9'))
			code = code * 10 + (str[i] - '0');
		else if (str[i] == ';')
		{
			apply_ansi_code(code);
			code = 0;
		}
		else if (str[i] == 'm')
		{
			apply_ansi_code(code);
			return (i + 1);
		}
		else
			return (i + 1);
		i ++;
	}
	return (i);
}


static void	vga_empty_at(size_t index)
{
	vga_write_entry_at(vga_make_entry(0, vga_fg, vga_bg), index);
}

void	vga_draw_cursor(void)
{
	vga_write_entry_at(vga_make_entry(CURSOR_CHAR, vga_fg, vga_bg), vga_row * VGA_WIDTH + vga_column);
}

void	vga_reset_cursor(void)
{
	cursor_tick = 0;
	vga_draw_cursor();
}

void	vga_set_cursor(size_t column, size_t row)
{
	size_t index = row * VGA_WIDTH + column;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (index & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (index >> 8) & 0xFF);
	
	vga_reset_cursor();
}

void	vga_toggle_cursor(void)
{
	cursor_tick = 0;
	if (cursor_toggle)
	{
		cursor_toggle = 0;
		vga_empty_at(vga_row * VGA_WIDTH + vga_column);
	}
	else
	{
		cursor_toggle = 1;
		vga_draw_cursor();
	}
}

void	vga_tick_cursor(void)
{
	cursor_tick ++;
	if (cursor_tick == 50000)	
		vga_toggle_cursor();
}


void	vga_line_scroll(void)
{
	size_t row;
	size_t column;
	
	row = 0;
	while (row < VGA_HEIGHT - 1)
	{
		column = 0;
		while (column < VGA_WIDTH)
		{
			vga_write_entry_at(vga_get_entry(column, row + 1), row * VGA_WIDTH + column);
			column ++;
		}
		row ++;
	}
	column = 0;
	while (column < VGA_WIDTH)
		vga_empty_at(row * VGA_WIDTH + (column ++));
	
}

void	vga_write_char(char uc)
{
	if (uc == '\n')
	{	
		vga_empty_at(vga_column + vga_row * VGA_WIDTH);			// erase cursor, if not newline will get replaced by the new char we're writing
	
		vga_column = 0;
		if (vga_row + 1 == VGA_HEIGHT)
			vga_line_scroll();
		else
			vga_row ++;
		vga_set_cursor(vga_column, vga_row);
		return ;
	}

	vga_write_entry_at(vga_make_entry(uc, vga_fg, vga_bg), vga_row * VGA_WIDTH + (vga_column ++));

	if (vga_column == VGA_WIDTH)
	{
		vga_column = 0;
		if (vga_row + 1 == VGA_HEIGHT)
			vga_line_scroll();
		else
			vga_row ++;
		
	}
	vga_set_cursor(vga_column, vga_row);
}

void	vga_write(char *str)
{
	unsigned int i;

	while (* str)
	{
		if (*str == '\033')
		{
			if (*(++ str) != '[')	// check si ansi code
				continue ;
			i = apply_ansi(++str);
			str += i;
		}
		else
			vga_write_char(*(str ++));
	}
}

void	vga_goto(size_t column, size_t row)
{
	vga_row = row;
	vga_column = column;
	vga_set_cursor(vga_column, vga_row);
}

void	vga_clear_screen(uint16_t *screen)
{
	size_t	i = 0;
	while (i < VGA_HEIGHT * VGA_WIDTH)
		screen[i ++] = vga_make_entry(' ', vga_fg, vga_bg);
}

void	vga_init_screens(void)
{
	vga_clear_screen((uint16_t *)VGA_BUFFER_ADDRESS);
	vga_clear_screen(alt_screen);
}

void	vga_switch_screen(void)
{
	size_t		index = 0;
	uint16_t	tmp;

	tmp = vga_row;
	vga_row = alt_row;
	alt_row = tmp;

	tmp = vga_column;
	vga_column = alt_column;
	alt_column = tmp;
	while (index < VGA_HEIGHT * VGA_WIDTH)
	{
		tmp = alt_screen[index];
		alt_screen[index] = ((uint16_t *)VGA_BUFFER_ADDRESS)[index];
		vga_write_entry_at(tmp, index ++);
	}
	vga_goto(vga_column, vga_row);
}

void	vga_backspace(void)
{
	if (vga_column == 0)
	{
		if (vga_row == 0)
		return ;
		
		vga_row --;
		vga_column = VGA_WIDTH - 1; 
	}
	else
		vga_column --;
	
	vga_empty_at(vga_column + vga_row * VGA_WIDTH);
	vga_empty_at(vga_column + vga_row * VGA_WIDTH + 1);
	
	vga_goto(vga_column, vga_row);
}
