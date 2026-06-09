section .text
	global vga_set_cursor


; curseur vga est une position 16 bit (position = offset dans le buffer 80x25 de vga)
; vga a ses propres registres internes, avec 2 ports exposés seulement (16 bits chacun, logique pour correspondra aux 16 bits de data):
; 	-> 0x3D4: numéro du registre a modifier
; 	-> 0x3D5: data que je veux ecrire/lire sur le registre (numero dans 0x3D4)
; CEPENDANT, registres vga sont de 8 bits (donc octet haut, octet bas) -> why not ?

vga_set_cursor:
; 	octet bas (registre vga 0x0F)

	; -> 0x3D4: registre a modifier = 0x0F
	mov dx, 0x3D4
	mov al, 0x0F
	out dx, al					; 0x0F = cursor location low (dans les registres vga, octet bas de l'index du cursor)

	; -> 0x3D5: valeur de l'octet bas
	mov dx, 0x3D5
	mov ecx, [esp + 4]
	out dx, cl


; 	octet haut (registre vga 0x0E)
	
	; -> 0x3D4: registre a modifier = 0x0F
	mov dx, 0x3D4
	mov al, 0x0E
	out dx, al 					; 0x0E = cursor location heigh (dans les registres vga, octet haut de l'index du cursor)

	; -> 0x3D5: valeur de l'octet haut
	
	mov dx, 0x3D5
	out dx, ch

	ret