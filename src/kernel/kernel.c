#include "kernel.h"

void	kernel_main(void *mboot_ptr)
{
	init_shells();
	serial_init_com1();

	print_multiboot_data(mboot_ptr);
	print_kernel_stack();

	active_keyboard();
}
