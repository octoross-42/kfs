#include "keyboard.h" 

unsigned char	scancode_to_ascii(uint8_t scancode) {
    if (scancode & 0x80) return 0;		// key release
    if (scancode >= sizeof(scancode_table)) return 0;
    return (scancode_table[scancode]);
}

void	active_keyboard(void)
{
	while (1)
	{
		while (!(inb(KEYBOARD_STATUS_PORT) & 0x01))
			;
		uint8_t sc = inb(KEYBOARD_DATA_PORT);
		unsigned char c = scancode_to_ascii(sc);
		if (c)
			vga_write_uchar(c);
	}
}
