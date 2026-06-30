section .text
	global strncmp

strncmp:
	push	edi
	push	esi

	mov		esi, [esp + 12]
	mov		edi, [esp + 16]
	mov		ecx, [esp + 20]
	
	xor		eax, eax
	xor		edx, edx
.loop:
	test	ecx, ecx
	jz		.return		; n = 0

	mov		al, [esi]
	mov		dl, [edi]
	sub		eax, edx
	
	test	dl, dl
	jz		.return		; s2[i] = 0 
	test	eax, eax	
	jnz		.return		; s1[i] != s2[i]
		 
	inc		esi
	inc		edi
	dec		ecx
	jmp		.loop
.return:
	pop esi
	pop edi
	ret