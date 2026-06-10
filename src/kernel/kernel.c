#include "vga.h"
#include "keyboard.h"

void	kfs1_screen(void)
{
	vga_goto((VGA_WIDTH / 2) - 4, VGA_HEIGHT / 2 - 2);
	vga_write((unsigned char *)"octor-os");
	vga_goto(0, VGA_HEIGHT - 5);
}

void	kernel_main(void)
{
	vga_init_screens();

	kfs1_screen();
	
	active_keyboard();
}
