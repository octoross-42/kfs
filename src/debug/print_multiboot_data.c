# include "debug.h"

typedef struct multiboot_mmap_entry_s
{
    uint32_t    size;     // taille de cette entrée (pas comptée dans size)
    uint64_t    addr;     // adresse de base
    uint64_t    len;      // longueur en bytes
    uint32_t    type;     // 1 = RAM libre, 2 = réservé
} __attribute__((packed)) multiboot_mmap_entry_t;

typedef struct multiboot_data_s
{
    uint32_t    flags;

    // bit 0
    uint32_t    mem_lower;
    uint32_t    mem_upper;

    // bit 1
    uint32_t    boot_device;

    // bit 2
    uint32_t    cmdline;

    // bit 3
    uint32_t    mods_count;
    uint32_t    mods_addr;

    // bit 4/5 — symboles (ignorés)
    uint32_t    syms[4];

    // bit 6
    uint32_t    mmap_length;
    uint32_t    mmap_addr;

    // bit 7
    uint32_t    drives_length;
    uint32_t    drives_addr;

    // bit 8
    uint32_t    config_table;

    // bit 9
    uint32_t    boot_loader_name;

    // bit 10
    uint32_t    apm_table;

    // bit 11
    uint32_t    vbe_control_info;
    uint32_t    vbe_mode_info;
    uint16_t    vbe_mode;
    uint16_t    vbe_interface_seg;
    uint16_t    vbe_interface_off;
    uint16_t    vbe_interface_len;

    // bit 12
    uint64_t    framebuffer_addr;
    uint32_t    framebuffer_pitch;
    uint32_t    framebuffer_width;
    uint32_t    framebuffer_height;
    uint8_t     framebuffer_bpp;
    uint8_t     framebuffer_type;
    uint8_t     color_info[6];
} __attribute__((packed)) multiboot_data_t; // pas de padding

void	print_multiboot_data(void *mboot_ptr)
{
	multiboot_data_t *mbd = (multiboot_data_t *)mboot_ptr;

	printk("\n");
	if (mbd->flags & (1 << 0))
	{
		printk("mem_lower: %u KB\n", mbd->mem_lower);
		printk("mem_upper: %u KB\n", mbd->mem_upper);
	}

	if (mbd->flags & (1 << 1))
		printk("boot_device: %x\n", mbd->boot_device);

	if (mbd->flags & (1 << 2))
		printk("cmdline: %s\n", (char *)mbd->cmdline);

	if (mbd->flags & (1 << 3))
		printk("mods_count: %u\n", mbd->mods_count);

	if (mbd->flags & (1 << 6))
	{
		printk("mmap:\n");
		multiboot_mmap_entry_t *entry = (multiboot_mmap_entry_t *)mbd->mmap_addr;
		while ((uint32_t)entry < mbd->mmap_addr + mbd->mmap_length)
		{
			printk("  base: %p  len: %x  \ttype: %u\n",
				(uint32_t)entry->addr,
				(uint32_t)entry->len,
				entry->type);
			entry = (multiboot_mmap_entry_t *)((uint32_t)entry + entry->size + sizeof(entry->size));
		}
	}
	printk("\n");
}
