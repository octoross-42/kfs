/* protected mode with GRUB -> VMA == LMA */

/* 0x00000000 – 0x000004FF   IVT + BDA (BIOS Data Area)
0x00000500	  – 0x00007BFF   libre (stack conventionnelle du BIOS)
0x00007C00 	  – 0x00007DFF   MBR / bootloader stage 1
0x00007E00 	  – 0x0009FFFF   libre (mais fragmenté, à éviter)
0x000A0000 	  – 0x000BFFFF   VGA framebuffer (0xB8000 sur 80x25 cellules)
0x000C0000 	  – 0x000FFFFF   ROM BIOS, zones réservées
0x00100000 	  – ...           ← GRUB charge le kernel ici -> convention */

ENTRY(_start)							/* def unique de _start dans le kernel */
OUTPUT_FORMAT(elf32-i386)

PHDRS
{
    rx PT_LOAD FLAGS(5);	/* R + X */
    rw PT_LOAD FLAGS(6);	/* R + W */
}

SECTIONS
{
    . = 0x100000;						/* convention */

    .multiboot	: { KEEP(*(.multiboot)) } :rx		/* GRUB oblige multiboot = premier*/
    .text		: { *(.text)						/* pas .text* par souci de rigueur -> section .textyouhou bon pas tres likely mais bon */
					*(.text.*) } :rx
	.rodata		: { *(.rodata) *(.rodata.*) } :rx
    .data		: { *(.data) *(.data.*) } :rw
    .bss		: { *(.bss) *(.bss.*) *(COMMON) } :rw

	/DISCARD/	: { *(.comment) *(.note.*) }
}