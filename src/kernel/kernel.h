#ifndef KERNEL_H
#define KERNEL_H

# include "keyboard.h"
# include "serial.h"
# include "kprint.h"
# include "ansi.h"
# include "gdt.h"

void	print_multiboot_data(void *mboot_ptr);

#endif
