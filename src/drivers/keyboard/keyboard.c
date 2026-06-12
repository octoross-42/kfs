#include "keyboard.h" 

static uint8_t			ctrl_pressed = 0;

char	scancode_to_ascii(uint8_t scancode)
{
	if (scancode & 0x80) return 0;		// key release
	if (scancode >= sizeof(scancode_table)) return 0;
	return (scancode_table[scancode]);
}

void	active_keyboard(void)
{
	char c;

	while (1)
	{
		vga_tick_cursor();
		while (!(inb(KEYBOARD_STATUS_PORT) & 0x01))
			vga_tick_cursor();
			
		uint8_t sc = inb(KEYBOARD_DATA_PORT);
		if (sc == 0x0E)
			vga_backspace();
		else if (sc == 0x1D)
		{
			ctrl_pressed = 1;
			c = 0;
		}
		else if (sc == 0x9D)
		{
			ctrl_pressed = 0;
			c = 0;
		}
		else if ((sc == 0x4D) || (sc == 0x4B)) // fleche droite ou gauche
		{	
			if (ctrl_pressed)
				vga_switch_screen();
		}
		else
		{
			c = scancode_to_ascii(sc);
			if (c)	
				vga_write_char(c);
		}
	}
}
