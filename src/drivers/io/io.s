section .text
	global outb
	global inb
outb:		; 8 bits
	mov dx, [esp + 4]	; dx 16 bits -> port
	mov al, [esp + 8]	; al 8 bits  -> valeur
	out dx, al
	ret

inb:
	mov dx, [esp + 4]
	xor eax, eax
	in al, dx
	ret