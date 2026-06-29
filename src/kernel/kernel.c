#include "kernel.h"

void	kernel_main(void *mboot_ptr)
{
	serial_init_com1();
	init_shells();

	print_multiboot_data(mboot_ptr);
	print_kernel_stack();

	active_keyboard();
}
