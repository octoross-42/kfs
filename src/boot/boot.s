; MULTIBOOT /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
; constants for multiboot header for grub, grub looks for them in the first 8KiB of the kernel file, at a 32 bits boundary 

section .multiboot
	align 4							; align multiboot header on 4 bytes -> first multiboot header byte (magic) is on address // 4 -> spec grub multiboot: header must be 32 bits aligned 
	dd 0x1BADB002					; magic number: 1 BAD BOOT
	dd 0x00000003					; flags = page alignment (ALIGN) and memory info (MEMINFO)
	dd -(0x1BADB002 + 0x00000003) ; checksum = -(MAGIC + FLAGS)


; STACK //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

; FONCTIONNEMENT DE LA STACK
; 	-> debut de la memoire (adresses basses) = stack_bottom,
; 	-> fin de la memoire (adresses hautes)   = stack_top,
; 	-> la stack s'empile vers les adresses basses (push),
; 	-> se depile vers les adresses hautes (pop)
; 	-> esp commence a stack_top, nouvelle variable = rapprochement vers stack_bottom
;	-> depassement (push de trop) de stack_bottom = stack overflow -> + de 16384 bytes de stack

; ADRESSES DE LA STACK
; reminder: labels are just the name of the current address when the assembler scans the line, at run time, labels dont exist anymore, just their address
; 	-> stack_bottom est la premiere adresse 16 bytes aligned
; 	-> stack_top est l'adresse 16384 bytes plus loin -> l'intérieur de la stack = 16384 bytes

global stack_top

section .bss
	align 16						; stack must be 16-byte aligned on x86
	stack_bottom:					
		resb 16384					; 4 * 4096 -> 4 "pages", taille standard de page sur x86: 4096 bytes (les memes pages de page tables de la pagination mmu/kernel)
	stack_top:


; ENTRY POINT ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

; at this point, we're in 32-bit protected mode on x86 (thanks grub)
; interrupts, paging are disabled, processus state is as defined in the multiboot standard
; there's only us here, and what we coded, so nothing much for the moment (nothing at all), but in return we're all powerful (no restrictions nor safeguards, scary)

extern kernel_main
extern gdt_init_flat

section .text
	global _start
	
_start:

	mov esp, stack_top				; on initialise la stack (esp), sinon comportement indéfini

	push ebx
	
	call gdt_init_flat
	; future pagination enabling, on n'a pas ca encore :)
	; future tout en fait, tout ce qui est concepts de code qui n'est pas basique (global, exception, etc)

	call kernel_main

	cli								; safety measure, kernel_main shouldnt return to here
freeze:	hlt							; pas de idt (interruption descriptor table) pour les nmi (non-masked interruptions), donc on halte toute activité, why not
	jmp freeze						; pas d'échapatoire au freeze, peu importe le nombre de nmi, on boucle sur le freeze (yipee) 
