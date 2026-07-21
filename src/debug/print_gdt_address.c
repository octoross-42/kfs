#include "debug.h"

struct gdt_ptr
{
	uint16_t limit;uint32_t base;
} __attribute__((packed));

extern void store_gdt(void *dst); 

void print_gdt_address(int fd)
{ 
	struct gdt_ptr gp;
	store_gdt(&gp);
	kfprintf(fd, "GDT base=0x%x limit=0x%x entries=%d\nExpected 0x800, 0x37\n",
           gp.base, gp.limit, (gp.limit + 1) / 8);
}