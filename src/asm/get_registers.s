section .text
	global get_esp
	global store_gdt

get_esp:
	mov eax, esp
	add eax, 4			; ret fera stack + 4
	ret

store_gdt:
    mov eax, [esp + 4]   ; 1er arg sur la stack
    sgdt [eax]           ; écrit 6 octets : limite(2) + base(4)
    ret
