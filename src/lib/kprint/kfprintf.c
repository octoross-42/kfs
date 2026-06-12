/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kfprintf.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: octoross <octoross@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:13:39 by octoross          #+#    #+#             */
/*   Updated: 2026/06/12 18:34:52 by octoross         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kprint.h"
#include "serial.h"

static void	uint32_base_aux(void (*write_char)(char), uint32_t n, const char *base, size_t len)
{
	// TTC - Trust The Caller
	//		base != NULL	-> segfault sur strlen
	//		base != ""		-> division par 0 (DE: Division error, exception cpu)
	// 		len(base) > 1	-> infinite loop 

	if (n >= len)
		uint32_base_aux(write_char, n / len, base, len);
	write_char(base[n % len]);
}

void	kfprint_uint32_base(void (*write_char)(char), uint32_t n, const char *base)
{	
	uint32_base_aux(write_char, n, base, strlen(base));
}

void	kfprint_uint32(void (*write_char)(char), uint32_t n)
{
	static const char * base = "0123456789";
	kfprint_uint32_base(write_char, n, base);
}

void	kfprint_int32(void (*write_char)(char), int32_t n)
{
	static const char * base = "0123456789";
	if (n < 0)
	{
		write_char('-');
		return kfprint_uint32_base(write_char, (uint32_t)-n, base);
	}
	kfprint_uint32_base(write_char, (uint32_t)n, base);
}

void kfprint_uint32_padded(void (*write_char)(char), uint32_t n, const char *base, size_t padding)
{
    size_t  len = strlen(base);
    size_t  digits = 1;
    uint32_t tmp = n / len;

    while ((tmp > 0) && (digits < padding ))
	{
		digits ++;
		tmp /= len;
	}

    while (digits < padding)
    {
        write_char(base[0]);	// padding
        padding--;
    }
    kfprint_uint32_base(write_char, n, base);
}


static void	write_with(void (*write_char)(char), const char *str)
{
	while (*str)
		write_char(*(str ++));
}

static void	map_format(void (*write_char)(char), char c, va_list *ap, int *i)
{
	if (c == 'c')
		return write_char(va_arg(*ap, int));
	if (c == 's')
	{
		char *str = va_arg(*ap, char *);
		if (!str)
			return write_with(write_char, "(null)");
		return write_with(write_char, str);
	}
	if (c == 'p')
	{
		write_with(write_char, "0x");
		return kfprint_uint32_padded(write_char, va_arg(*ap, uint32_t), "0123456789abcdef", 8);
	}
	if ((c == 'd') || (c == 'i'))
		return kfprint_int32(write_char, va_arg(*ap, int32_t));
	if (c == 'u')
		return kfprint_uint32(write_char, va_arg(*ap, uint32_t));
	if (c == 'x')
		return kfprint_uint32_base(write_char, va_arg(*ap, uint32_t), "0123456789abcdef");
	if (c == 'X')
		return kfprint_uint32_base(write_char, va_arg(*ap, uint32_t), "0123456789ABCDEF");
	if (c == '%')
		return write_char('%');
	(*i)--;
	write_char('%');
}

void	kfvprintf(void (*write_char)(char), const char *format, va_list ap)
{
	// TTC - Trust The Caller
	// 		-> write_char adresse valide
	// 		-> format adresse valide
	int	i;
	
	i = 0;
	while (format[i])
	{
		if (format[i] == '%')
			map_format(write_char, format[++ i], &ap, &i);
		else
			write_char(format[i]);
		i ++;
	}
}

void	kfprintf(void (*write_char)(char), const char *format, ...)
{
	// TTC - Trust The Caller
	// 		-> write_char adresse valide
	// 		-> format adresse valide
	va_list	ap;
	int	i;

	va_start(ap, format);
	i = 0;
	while (format[i])
	{
		if (format[i] == '%')
			map_format(write_char, format[++ i], &ap, &i);
		else
			write_char(format[i]);
		i ++;
	}
	va_end(ap);
}
