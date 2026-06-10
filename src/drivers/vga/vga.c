#include "vga.h"

// vga a un text buffer à l'adresse physique 0xB8000
// 	-> buffer = grille de 80x25 = 2000 cellules
// 	-> cellule = 16 bytes -> [octet 1: attribut -> [bg 4 bits, fg bits]] [octet 0: ascii] (little endian, mais on gere pas, le compilateur gere, visible depuis gdb)

static size_t			vga_row = 0;
static size_t			vga_column = 0;
static enum vga_color	vga_fg = VGA_COLOR_DARK_GREY;
static enum vga_color	vga_bg = VGA_COLOR_BLACK;

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
			vga_write_entry_at(vga_get_entry_at(column, row + 1), column, row);
			column ++;
		}
		row ++;
	}
	column = 0;
	while (column < VGA_WIDTH)
		vga_write_entry_at(vga_make_entry(' ', vga_fg, vga_bg), column ++, row);
	
}

void	vga_write_uchar(unsigned char uc)
{
	if (uc == '\n')
	{
		vga_column = 0;
		if (++vga_row == VGA_HEIGHT)
			vga_row = 0;
		vga_set_cursor(vga_column, vga_row);
		return ;
	}

	vga_write_entry_at(vga_make_entry(uc, vga_fg, vga_bg), vga_column ++, vga_row);

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
