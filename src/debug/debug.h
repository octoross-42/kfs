#ifndef DEBUG_H 
#define DEBUG_H

# include "ansi.h"
# include "kprint.h"

uint32_t	get_esp(void);
void		print_kernel_stack(int fd);

void		print_gdt_address(int fd);

void		print_multiboot_data(void *mboot_ptr);

#endif