#ifndef KEYBOARD_H

# define KEYBOARD_H

# include "io.h"
# include "vga.h"

static const unsigned char scancode_table[] = {
	0,   0,  '1', '2', '3', '4', '5', '6',
	'7', '8', '9', '0', '-', '=',  0,   0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
	'o', 'p', '[', ']', '\n',  0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`',  0, '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/',  0,   0,
	0,  ' '                                   
};

void	active_keyboard(void);

# define KEYBOARD_DATA_PORT		0x60
# define KEYBOARD_STATUS_PORT	0x64


#endif