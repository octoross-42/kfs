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

void	line_scroll(void)
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

void	vga_write_uchar(unsigned char uc)
{
	if (uc == '\n')
	{
		vga_column = 0;
		if (vga_row + 1 == VGA_HEIGHT)
			line_scroll();
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
			line_scroll();
		else
			vga_row ++;
		
	}
	vga_set_cursor(vga_column, vga_row);
}

void	vga_write(unsigned char *str)
{
	unsigned int i = 0;
	while (str[i])
		vga_write_uchar(str[i ++]);
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
