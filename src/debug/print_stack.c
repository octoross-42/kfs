/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_stack.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: octoross <octoross@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 17:05:00 by octoross          #+#    #+#             */
/*   Updated: 2026/07/21 18:08:17 by octoross         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"

extern uint32_t stack_top;

void	print_kernel_stack(int fd)
{
	uint32_t	esp = get_esp();
	uint32_t	top = (uint32_t)&stack_top;

	kfprintf(fd, "\n%sKernel Stack\n\n", ansi(BRIGHT, END));
	kfprintf(fd, "Adresse       %sValeur%s\n", ansi(CYANFG, END), ansi(RESET, END));
	while (esp < top)
	{
		kfprintf(fd, "%p    %s%p%s\n", esp, ansi(BRIGHTOFF, CYANFG, END), *((uint32_t *)esp), ansi(RESET, END));
		esp += 4;
	}
	kfprintf(fd, "\n");
}
