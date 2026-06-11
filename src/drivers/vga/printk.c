/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printk.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: octoross <octoross@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:13:39 by octoross          #+#    #+#             */
/*   Updated: 2026/06/11 16:36:06 by octoross         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "vga.h"

static void	printk_format(char c, va_list *ap, int *i)
{
	if (c == 'c')
		return vga_write_uchar((unsigned char)va_arg(*ap, int));
	if (c == 's')
	{
		char *str = va_arg(*ap, char *);
		if (!str)
			return vga_write("(null)");
		return vga_write(str);
	}
	if (c == 'p')
	{
		vga_write("0x");
		return vga_write_uint32_padded(va_arg(*ap, uint32_t), "0123456789abcdef", 8);
	}
	if ((c == 'd') || (c == 'i'))
		return vga_write_int32(va_arg(*ap, int32_t));
	if (c == 'u')
		return vga_write_uint32(va_arg(*ap, uint32_t));
	if (c == 'x')
		return vga_write_uint32_base(va_arg(*ap, uint32_t), "0123456789abcdef");
	if (c == 'X')
		return vga_write_uint32_base(va_arg(*ap, uint32_t), "0123456789ABCDEF");
	if (c == '%')
		return vga_write_uchar('%');
	(*i)--;
	vga_write_uchar('%');
}

void	printk(const char *format, ...)
{
	va_list	ap;
	int		i;

	if (!format)
		return ;
	va_start(ap, format);
	i = 0;
	while (format[i])
	{
		if (format[i] == '%')
			printk_format(format[++ i], &ap, &i);
		else
			vga_write_uchar(format[i]);
		i ++;
	}
	va_end(ap);
}
