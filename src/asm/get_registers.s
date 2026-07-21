section .text
	global get_esp

get_esp:
	mov eax, esp
	add eax, 4			; ret fera stack + 4
	ret
