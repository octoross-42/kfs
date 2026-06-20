/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_stack.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: octoross <octoross@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 17:05:00 by octoross          #+#    #+#             */
/*   Updated: 2026/06/20 02:39:59 by octoross         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gdt.h"

extern uint32_t stack_top;

void	print_kernel_stack(void)
{
	uint32_t	esp = get_esp();
	uint32_t	top = (uint32_t)&stack_top;

	printk("\n%sKernel Stack\n\n", ansi(BRIGHT, END));
	printk("Adresse   \t%sValeur%s\n", ansi(BLUEFG, END), ansi(RESET, END));
	while (esp < top)
	{
		printk("%p\t%s%p%s\n", esp, ansi(BRIGHTOFF, BLUEFG, END), *((uint32_t *)esp), ansi(RESET, END));
		esp += 4;
	}
	printk("\n");
}
