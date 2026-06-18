/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: octoross <octoross@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/15 19:34:50 by octoross          #+#    #+#             */
/*   Updated: 2026/06/18 13:50:27 by octoross         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gdt.h"

static void	gdt_set_entry(gdt_entry_t *e,
						uint32_t base,
						uint32_t limit,
						uint8_t access,
						uint8_t limit_flags)
{
	// RAPPEL 1 hexa = 4 bits, 2 hexa = 8 bits, 4 hexa = 16 bits
	e->limit_low		= limit & 0xFFFF;		// 0xFFFF -> 16 derniers bits
    e->base_low			= base  & 0xFFFF;		// same
	
    e->base_mid			= (base >> 16) & 0xFF;	// 8 bits du milieu (on enleve les 16 derniers bits, et on prend les 8 derniers)
    e->access			= access;
    e->flags_limit_high	= ((limit_flags & 0x0F) << 4) | ((limit >> 16) & 0x0F);
	e->base_high		= (base >> 24) & 0xFF;
}

static gdt_entry_t *gdt = (gdt_entry_t *)GDT_BASE;
gdtr_t	gdtr;

void gdt_init_flat(void)
{
	// 0: Null descriptor — obligatoire, pour pouvoir fault quand segment adress = 0
    gdt_set_entry(&gdt[0], 0, 0, 0, 0);

    gdt_set_entry(&gdt[1], 0, 0xFFFFFFFF, KERNEL_CODE_SEGMENT_ACCESS, SEGMENT_LIMIT_FLAGS);	// Kernel Code
    gdt_set_entry(&gdt[2], 0, 0xFFFFFFFF, KERNEL_DATA_SEGMENT_ACCESS, SEGMENT_LIMIT_FLAGS);	// Kernel Data
    gdt_set_entry(&gdt[3], 0, 0xFFFFFFFF, KERNEL_DATA_SEGMENT_ACCESS, SEGMENT_LIMIT_FLAGS);	// Kernel Stack

    gdt_set_entry(&gdt[4], 0, 0xFFFFFFFF, USER_CODE_SEGMENT_ACCESS, SEGMENT_LIMIT_FLAGS);	// User Code
    gdt_set_entry(&gdt[5], 0, 0xFFFFFFFF, USER_DATA_SEGMENT_ACCESS, SEGMENT_LIMIT_FLAGS);	// User Data
    gdt_set_entry(&gdt[6], 0, 0xFFFFFFFF, USER_DATA_SEGMENT_ACCESS, SEGMENT_LIMIT_FLAGS);	// User Stack
	
	gdtr.limit = sizeof(gdt_entry_t) * GDT_ENTRIES - 1;
    gdtr.base  = (uint32_t)gdt;

    gdt_flush();
}


// BASE
// 1111 1111 1111 1111 1111 1111 1111 1111
// ^	   ^ ^       ^ ^                 ^
// high      mid       low
// 8b, 2hex  8b, 2hex  16b, 4hex

// LIMIT
// 0000 0000 0000 1111 1111 1111 1111 1111
//                ^  ^ ^                 ^
//                high low
// 				  F    0    0    0    0