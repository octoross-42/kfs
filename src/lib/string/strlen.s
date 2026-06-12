section .text
    global strlen
	global false_strlen


strlen:
; 	TTC: Trust The Caller
; 		-> s is a valid pointer (doesnt segfault)
;		-> s is not null
; 		-> s is '\0' terminated
	mov ecx, [esp + 4]
	xor eax, eax
	.loop:
		mov dl, [ecx + eax]
		test dl, dl
		jz .return
		inc eax
		jmp .loop
	.return:
		ret
		