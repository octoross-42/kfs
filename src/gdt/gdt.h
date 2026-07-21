#ifndef GDT_H
#define GDT_H

#include "types.h"

// CPL - Current Privilege Level
// DPL - Descriptor Permissions Level
// CS (code segment), DS (data segment), SS (stack segment), ES, FS, GS -> usage général, pas pour le moment
// GPF - General Protection Fault, exception numéro 13 (#GP) -> pas pour le moment

// Octet 7		   Octet 6       	 		Octet 5			  Octets 4-2	 Octets 1-0
// [Base 31:24]	   [G|D|L|AVL|Lim 19:16]	[P|DPL|S|Type]	  [Base 23:0]	 [Limit 15:0]

// ACCESS
// bit 7:	P	- Present
// 			-> P = 1
// 			-> si P = 0, Segment Not Present exception (#NP), utile pour:
// 				-> swapping: marquer un segment P = 0 => cpu leve exception et kernel recharge le segment en RAM, puis remet P = 1
// 				-> inutilisé: on a réservé un segment mais on n'a encore rien écrit, empeche utilisation accidentelle
// bit 6-5	DPL	- Descriptor privilege limits
// 			-> 0 = kernel
// 			-> 3 = user 
// bit 4	S	- Segment Descriptor type -> is this a standart segment ? (yes -> code/data or no -> systeme)
// 			-> S = 1 -> segment code/data -> programmes
// 			-> S = 0 -> segment systeme, structures internes au CPU (TSS, LDT, call gate, trap gate) -> on ne connait pas encore
// bit 3-0	Type (autorisations:)
// 			-> bit 3 = 0	data	(RO, RO + Accessed, R/W, R/W + Accessed, RO + Expand-Down, etc)
// 			-> bit 3 =1		code	(Execute-Only, EO + Accessed, E/W + Accessed, EO + Conforming, etc)
// 				-> Access = bit de access, =1 quand le segment est utilisé, utile pour swapping
// 				-> Expand down = segment de stack -> généralement pas utilisé
// 				-> Conforming = segment de code, utilsé pour code partagé kernel/user
// 		-> pour kfs-2, 0xA (Execute/Read) et 0x2 (Read/Write) utilisés seulement


// little-endian + convention intel -> [bit point fort: bit point faible]
// 									-> placement en bits dans la struct de l'objet
// Le découpage est dégueulasse, bouh intel (ya une une conversion d'architecture askip...)
// 	-> les valeurs sont découpées sur plusieurs sous-valeurs qui représentent des parties de valeurs différentes
// 		-> LIMIT = limit_low + flags_limit
// 		-> BASE = base_low + base_mid + base_high



// BASE
// 	-> adresse du segment
// LIMIT
// 	-> offset du segment

// Kernel Code : P=1, DPL=0 -> kernel, S=1 -> code/data, Type=10 (0x0A)-> code execute/read
// 1 00 1 1010 = 128 + 0 + 16 + 10 = 154 = 144 (9*16) + 10 = 0x9A
#define KERNEL_CODE_SEGMENT_ACCESS  0x9A

// Kernel Data : P=1, DPL=0, S=1, Type=2 (0x02) -> data read/write
// 1 00 1 0010 = 128 + 16 + 2 = 146 = 144 (9 * 16) + 2 = 0x92
#define KERNEL_DATA_SEGMENT_ACCESS  0x92

// User Code : P=1, DPL=3, S=1, Type=0xA
// 1 11 1 1010 = 0xFA
#define USER_CODE_SEGMENT_ACCESS    0xFA

// User Data : P=1, DPL=3, S=1, Type=0x2
// 1 11 1 0010 = 0xF2
#define USER_DATA_SEGMENT_ACCESS    0xF2


// LIMITES FLAGS
// 	- G - Granularity, l'unité de la limite, par quoi on multiplie la limite
// 		-> 1 octet pour G = 0
// 		-> 4096 octets pour G = 1
// 	- D/B - Default/Big
// 		-> D/B=0 -> opérations 16 bits par défaut sur ce segment -> vielle archi 16 bits
// 		-> D/B=1 -> opérations 32 bits par défaut sur ce segment -> c'est nous ca
// 	- L - Long Mode -> L = 1 pour long mode, nous L = 0 pour protected mode
// - AVL - Available -> rien, bit libre pour l'OS, CPU l'ignore -> 0

// G = 1, D/B = 1, L = 0, AVL = 0 -> 1100 = 12 = 0xC
#define SEGMENT_LIMIT_FLAGS			0xC

typedef struct
{
	// limit[15:0]
	uint16_t limit_low;
	// base[15:0]
	uint16_t base_low;
	// base[23:16]
	uint8_t  base_mid;
	uint8_t  access;
	// limit flags + limit[19:16]
	uint8_t  flags_limit_high;	// G | D | L | AVL
	// base[31:24]
	uint8_t  base_high;
} __attribute__((packed)) gdt_entry_t;

#define GDT_BASE    0x00000800
#define GDT_ENTRIES 7

typedef struct {
    uint16_t limit;		// taille de la GDT en octets - 1 (index du dernier octet valide)
    uint32_t base;		// adresse de la GDT en RAM
} __attribute__((packed)) gdtr_t;


void	gdt_init_flat(void);
void	gdt_flush(void);

# include "kprint.h"

#endif