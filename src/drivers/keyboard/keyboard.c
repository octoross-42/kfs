static const char scancode_table[] = {
	0,   0,  '1', '2', '3', '4', '5', '6',
	'7', '8', '9', '0', '-', '=',  0,   0,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
	'o', 'p', '[', ']', '\n',  0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`',  0, '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/',  0,   0,
	0,  ' '                                   
};

#include "types.h"

char	scancode_to_ascii(uint8_t scancode) {
    if (scancode & 0x80) return 0;					// key release
    if (scancode >= sizeof(scancode_table)) return 0;
    return (scancode_table[scancode]);
}
