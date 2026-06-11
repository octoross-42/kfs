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

void	vga_change_fg(enum vga_color fg) { vga_fg = fg; }
void	vga_change_bg(enum vga_color bg) { vga_bg = bg; }

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
		vga_write_entry_at(vga_make_entry(' ', vga_fg, vga_bg), row * VGA_WIDTH + (column ++));
	
}

static void	vga_write_uint32_base_aux(uint32_t n, const char *base, size_t len)
{
	if (n >= len)
		vga_write_uint32_base_aux(n / len, base, len);
	vga_write_uchar(base[n % len]);
}

void	vga_write_uint32_base(uint32_t n, const char *base)
{
	// TTC - Trust The Caller
	//		base != NULL	-> segfault sur strlen
	//		base != ""		-> division par 0 (DE: Division error, exception cpu)
	// 		len(base) > 1	-> infinite loop 
	vga_write_uint32_base_aux(n, base, strlen(base));
}

void	vga_write_uint32(uint32_t n)
{
	static const char * base = "0123456789";
	vga_write_uint32_base(n, base);
}

void	vga_write_int32(int32_t n)
{
	static const char * base = "0123456789";
	if (n < 0)
	{
		vga_write_uchar('-');
		return vga_write_uint32_base((uint32_t)-n, base);
	}
	vga_write_uint32_base((uint32_t)n, base);
}

void vga_write_uint32_padded(uint32_t n, const char *base, size_t padding)
{
    size_t  len = strlen(base);
    size_t  digits = 1;
    uint32_t tmp = n / len;

    while ((tmp > 0) && (digits < padding ))
	{
		digits ++;
		tmp /= len;
	}

    while (digits < padding)
    {
        vga_write_uchar(base[0]);	// padding
        padding--;
    }
    vga_write_uint32_base(n, base);
}

void	vga_write_uchar(unsigned char uc)
{
	if (uc == '\n')
	{
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
	unsigned int i = 0;
	while (str[i])
		vga_write_uchar((unsigned char)str[i ++]);
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
	
	vga_write_entry_at(vga_make_entry(' ', vga_fg, vga_bg), vga_column + vga_row * VGA_WIDTH);
	vga_goto(vga_column, vga_row);
}
