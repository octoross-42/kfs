#include "vga.h"

void	clean_screen(void)
{
	size_t	i = 0;
	while (i ++ < VGA_HEIGHT * VGA_WIDTH)
		vga_write_char(' ');
}

void	kernel_main(void)
{
	clean_screen();
	vga_goto((VGA_WIDTH / 2) - 4, VGA_HEIGHT / 2 - 1);
	vga_write((unsigned char *)"octor-os");
}