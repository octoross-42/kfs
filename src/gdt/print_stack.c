/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_stack.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: octoross <octoross@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 17:05:00 by octoross          #+#    #+#             */
/*   Updated: 2026/06/20 01:00:16 by octoross         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gdt.h"

extern uint32_t stack_top;

void	print_kernel_stack(void)
{
	// uint32_t	esp = get_esp();
	// uint32_t	top = (uint32_t)&stack_top;

	printk("Kernel Stack");
	
}