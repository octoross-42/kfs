#include "keyboard.h"
#include "serial.h"

void	kfs1_screen(void)
{
	vga_goto((VGA_WIDTH / 2) - 1, VGA_HEIGHT / 2 - 2);
	vga_write("42");

	serial_write_com1("halllloooooooooo");
}

// uint32_t multiboot_data[32];

// void main(void *mboot_ptr)

void	kernel_main(void)
{
	vga_init_screens();
	serial_init_com1();

	kfs1_screen();

	active_keyboard();
}
