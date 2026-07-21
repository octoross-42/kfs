extern gdtr
extern stack_top

section .text
	global gdt_flush

gdt_flush:
	; on reload la table gdt
	lgdt [gdtr]

	; reload CS (code segment -> kernel)
	jmp 0x08:.reload_cs	; 0x08 -> kernel code, index 1 de la gdt (offset de 8 bits depuis l'adresse de la gdt)

.reload_cs:
	; ax parce registres de segment ne peuvent pas recevoir de valeur immédiate
    mov ax, 0x10		; 0x10 -> kernel data, index 2	
	; segment par defaut pour instructions data (mov)
    mov ds, ax
    ; segment de destination pour copie de chaines (movs, stos)
	mov es, ax
	; libre usage, souvent pour thread local storage (TLS)
    mov fs, ax
	; libre usage, souvent pour structures de kernel internes par CPU
    mov gs, ax

	mov ax, 0x18		; 0x18 -> kernel stack, index 3
	; ne recharge pas la stack, juste le descriptor de kernel stack
    mov ss, ax
    ret
