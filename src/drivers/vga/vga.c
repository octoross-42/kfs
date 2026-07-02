#include "vga.h"

// vga a un text buffer à l'adresse physique 0xB8000
// 	-> buffer = grille de 80x25 = 2000 cellules
// 	-> cellule = 16 bytes -> [octet 1: attribut -> [bg 4 bits, fg bits]] [octet 0: ascii] (little endian, mais on gere pas, le compilateur gere, visible depuis gdb)

// static enum vga_color	active->fg = VGA_COLOR_DARK_GREY;
// static enum vga_color	active->bg = VGA_COLOR_BLACK;

static screen_t			screens[NBR_SCREENS];
static screen_t			*active = &screens[0];

static uint8_t			cursor_toggle = 1;
static uint32_t			cursor_tick = 0;

extern shell_t			shells[NBR_SCREENS];
extern shell_t			*active_shell;

void	vga_change_fg(int screen_nbr, enum vga_color fg) { if (screen_nbr < 0) active->fg = fg; else screens[screen_nbr].fg = fg; }
void	vga_change_bg(int screen_nbr, enum vga_color bg) { if (screen_nbr < 0) active->bg = bg; else screens[screen_nbr].bg = bg; }

enum vga_color	vga_get_fg(int screen_nbr) { if (screen_nbr < 0) return (active->fg); return (screens[screen_nbr].fg); }
enum vga_color	vga_get_bg(int screen_nbr) { if (screen_nbr < 0) return (active->bg); return (screens[screen_nbr].bg); }

size_t	vga_get_row(int screen_nbr) { if (screen_nbr < 0) return (active->row); return (screens[screen_nbr].row); }
size_t	vga_get_col(int screen_nbr) { if (screen_nbr < 0) return (active->col); return (screens[screen_nbr].col); }

static screen_t	*get_screen(int screen_nbr)
{
	if (screen_nbr < 0)
		return (active);
	else
		return (&screens[screen_nbr]);	
}

static void	vga_draw_entry_on(screen_t *screen, uint16_t entry, size_t index)
{
	if (screen == active)
		((uint16_t *)VGA_BUFFER_ADDRESS)[index] = entry;
	screen->entries[index] = entry;
}

static void	vga_draw_char_on(screen_t *screen, char c, size_t index)
{
	uint16_t	entry;

	entry = vga_make_entry((unsigned char)c, screen->fg, screen->bg);
	if (screen == active)
		((uint16_t *)VGA_BUFFER_ADDRESS)[index] = entry;
	screen->entries[index] = entry;
}


static void	apply_ansi_code(int screen_nbr, unsigned int code)
{
	screen_t	*screen;
	enum vga_color bg;

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
		vga_change_fg(screen_nbr, VGA_RESET_FG);
		return vga_change_bg(screen_nbr, VGA_RESET_BG);
	}
	if (code == 1)						// bold
		return vga_change_fg(screen_nbr, active->fg | 0b1000);	// bit 3 = bit d'intensité
	if ((code == 2) || (code == 22))	// dim
		return vga_change_fg(screen_nbr, active->fg & 0b0111);	// bit 3 = bit d'intensité
	if (code == 5)						// blink ?
		return vga_change_bg(screen_nbr, active->bg | 0b1000);	// bit 7 = bit de blink (entry = bg (4bits) + fg (4bits) + c (8 bits)), pour styliser -> 8 bits
	if (code == 7)						// reverse
	{
		if (screen_nbr < 0)
			screen = active;
		else
			screen = &screens[screen_nbr];
		bg = screen->bg;	
		vga_change_bg(screen_nbr, screen->fg);
		return vga_change_fg(screen_nbr, bg);
	}
	if ((30 <= code) && (code <= 37))
		return vga_change_fg(screen_nbr, ansi_to_vga[code - 30]);
	if ((40 <= code) && code <= 47)
		return vga_change_bg(screen_nbr, ansi_to_vga[code - 40]);
	if ((90 <= code) && (code <= 97))
		return vga_change_fg(screen_nbr, ansi_to_vga[code - 90] + 8);
	if ((100 <= code) && code <= 107)
		return vga_change_bg(screen_nbr, ansi_to_vga[code - 100] + 8);
	return ;
}

static unsigned int	apply_ansi(int screen_nbr, char *str)
{
	unsigned int i = 0;
	unsigned int code = 0;
	while (str[i])
	{
		if (('0' <= str[i]) && (str[i] <= '9'))
			code = code * 10 + (str[i] - '0');
		else if (str[i] == ';')
		{
			apply_ansi_code(screen_nbr, code);
			code = 0;
		}
		else if (str[i] == 'm')
		{
			apply_ansi_code(screen_nbr, code);
			return (i + 1);
		}
		else
			return (i + 1);
		i ++;
	}
	return (i);
}


static inline void	vga_empty_at(screen_t *screen, size_t index)
{
	vga_draw_char_on(screen, 0, index);
}

void	vga_draw_cursor(void)
{
	static const uint16_t entry = (uint16_t)(((VGA_COLOR_BLACK << 4 | VGA_COLOR_LIGHT_GREY) << 8) | CURSOR_CHAR);
	vga_draw_entry_on(active, entry, active->row * VGA_WIDTH + active->col);
}

inline void	vga_reset_cursor(void)
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

static void	vga_toggle_cursor(void)
{
	cursor_tick = 0;
	if (cursor_toggle)
	{
		cursor_toggle = 0;
		vga_empty_at(active, active->row * VGA_WIDTH + active->col);
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
	if (cursor_tick == VGA_CURSOR_TICKRATE)	
		vga_toggle_cursor();
}

void	vga_line_scroll(screen_t *screen)
{
	size_t row;
	size_t column;
	
	row = 0;
	while (row < VGA_HEIGHT - 1)
	{
		column = 0;
		while (column < VGA_WIDTH)
		{
			vga_draw_entry_on(screen, vga_get_entry(column, row + 1), row * VGA_WIDTH + column);
			column ++;
		}
		row ++;
	}
	column = 0;
	while (column < VGA_WIDTH)
		vga_empty_at(screen, row * VGA_WIDTH + (column ++));
}

void	shell_line_scroll(screen_t *screen)
{
	size_t row;
	size_t column;
	
	if (active_shell->row == 0)
		active_shell->col = 0;
	else
		active_shell->row --;
	
	row = 0;
	while (row < VGA_HEIGHT - 1)
	{
		column = 0;
		while (column < VGA_WIDTH)
		{
			vga_draw_entry_on(screen, vga_get_entry(column, row + 1), row * VGA_WIDTH + column);		
			column ++;
		}
		row ++;
	}
	column = 0;
	while (column < VGA_WIDTH)
		vga_empty_at(screen, row * VGA_WIDTH + (column ++));
}

void	vga_write_char_no_cursor(screen_t *screen, char c)
{
	if (c == '\n')
	{
		vga_empty_at(screen, screen->col + screen->row * VGA_WIDTH);			// erase cursor, if not newline will get replaced by the new char we're writing
	
		screen->col = 0;
		if (screen->row + 1 == VGA_HEIGHT)
			shell_line_scroll(screen);
		else
			screen->row ++;
		return ;
	}

	vga_draw_char_on(screen, c, screen->row * VGA_WIDTH + (screen->col ++));

	if (screen->col == VGA_WIDTH)
	{
		screen->col = 0;
		if (screen->row + 1 == VGA_HEIGHT)
			shell_line_scroll(screen);
		else
			screen->row ++;
	}
}


void	vga_write_char(int screen_nbr, char c)
{
	screen_t	*screen;

	screen = get_screen(screen_nbr);
	vga_write_char_no_cursor(screen, c);
	if (screen == active)
		vga_set_cursor(active->col, active->row);
}

void	vga_write(int screen_nbr, char *str)
{
	unsigned int	i;
	screen_t		*screen;

	screen = get_screen(screen_nbr);
	while (* str)
	{
		if (*str == '\033')
		{
			if (*(++ str) != '[')	// check si ansi code
				continue ;
			i = apply_ansi(screen_nbr, ++str);
			str += i;
		}
		else
			vga_write_char_no_cursor(screen, *(str ++));
	}
	
	if (screen == active)
		vga_set_cursor(active->col, active->row);
}

void	vga_goto(size_t column, size_t row)
{
	active->row = row;
	active->col = column;
	vga_set_cursor(active->col, active->row);
}

void	vga_clear_screen(int screen_nbr)
{
	size_t		i;
	screen_t	*screen;

	i = 0;
	screen = get_screen(screen_nbr);
	while (i < VGA_SIZE)
		vga_draw_char_on(screen, ' ', i ++);
}

void	vga_init_screen(unsigned int screen_nbr)
{
	screen_t	*screen;

	vga_clear_screen(screen_nbr);
	screen = &screens[screen_nbr];
	screen->fg = VGA_RESET_FG;
	screen->bg = VGA_RESET_BG;
	screen->col = 0;
	screen->row = 0;
}

static void vga_refresh_screen(void)
{
	int		i;
	
	i = -1;
	while (++ i < VGA_SIZE)
		vga_draw_entry_on(active, active->entries[i], i);
	vga_goto(active->col, active->row);
}

void	vga_next_screen(void)
{
	if (active == screens + NBR_SCREENS - 1)
		active = screens;
	else
		active ++;
	vga_refresh_screen();	
}

void	vga_prev_screen(void)
{
	if (active == screens)
		active = screens + NBR_SCREENS - 1;
	else
		active --;
	vga_refresh_screen();
}


void	vga_backspace(void)
{
	if (active->col == 0)
	{
		if (active->row == 0)
		return ;
		
		active->row --;
		active->col = VGA_WIDTH - 1; 
	}
	else
		active->col --;
	
	vga_empty_at(active, active->col + active->row * VGA_WIDTH);
	vga_empty_at(active, active->col + active->row * VGA_WIDTH + 1);
	
	vga_goto(active->col, active->row);
}

int	vga_backspace_shell(void)
{
	if ((active_shell->row < active->row) && (active->col == 0))
	{
		if (active->row == 0)
			return (0);
		active->row --;
		active->col = VGA_WIDTH - 1; 
	}
	else if ((active_shell->row == active->row) && (active->col <= active_shell->col))
		return (0);
	else
		active->col --;
	
	vga_empty_at(active, active->col + active->row * VGA_WIDTH);
	vga_empty_at(active, active->col + active->row * VGA_WIDTH + 1);
	
	vga_goto(active->col, active->row);
	return (1);
}
