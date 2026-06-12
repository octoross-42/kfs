/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printk.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: octoross <octoross@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 16:09:12 by octoross          #+#    #+#             */
/*   Updated: 2026/06/12 17:04:45 by octoross         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "kprint.h"

void	printk(const char *format, ...)
{
	static void (* const write_char)(char) = serial_write_char_com1;
    va_list ap;
	
    va_start(ap, format);
    kfvprintf(write_char, format, ap);
    va_end(ap);
}
