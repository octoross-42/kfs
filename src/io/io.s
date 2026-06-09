section .text
	global outb
outb:		; 8 bits
	mov dx, [esp + 4]
	mov al, [esp + 8]
	out dx, al
	ret
