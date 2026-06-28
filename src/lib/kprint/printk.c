/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printk.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: octoross <octoross@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 16:09:12 by octoross          #+#    #+#             */
/*   Updated: 2026/06/28 19:40:38 by octoross         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "kprint.h"

void	printk(const char *format, ...)
{
	static int fd = NBR_SCREENS;
    va_list ap;
	
    va_start(ap, format);
    kfvprintf(fd, format, ap);
    va_end(ap);
}
