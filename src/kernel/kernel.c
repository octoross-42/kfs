#include "kernel.h"

void	kfs1_screen(void)
{
	vga_goto((VGA_WIDTH / 2) - 1, VGA_HEIGHT / 2 - 2);
	vga_write("42");
}


void	kernel_main(void *mboot_ptr)
{
	vga_init_screens();
	serial_init_com1();

	kfs1_screen();

	print_multiboot_data(mboot_ptr);


	active_keyboard();
}
