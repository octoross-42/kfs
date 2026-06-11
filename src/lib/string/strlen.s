section .text
    global strlen


strlen:
; 	TTC: Trust The Caller
; 		-> s is a valid pointer (doesnt segfault)
;		-> s is not null
; 		-> s is '\0' terminated  
	xor eax, eax
	.loop:
		mov dl, [esp + 4 + eax]
		test dl, dl
		jz .return
		inc eax
		jmp .loop
	.return:
		ret
