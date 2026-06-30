/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kfprintf.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: octoross <octoross@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 13:13:39 by octoross          #+#    #+#             */
/*   Updated: 2026/06/30 15:31:59 by octoross         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kprint.h"
#include "serial.h"

static void write_char(int fd, char c)
// 	-1				-> active screen
// 0 - NBR_SCREENS	-> screen i
// NBR_SCREENS		-> serial com 1
{
	if (fd < NBR_SCREENS)
		return vga_write_char(fd, c);
	if (fd == NBR_SCREENS)
		return serial_write_char_com1(c);
}


static void write(int fd, char *s)
// 	-1				-> active screen
// 0 - NBR_SCREENS	-> screen i
// NBR_SCREENS		-> serial com 1
{
	if (fd < NBR_SCREENS)
		return vga_write(fd, s);
	if (fd == NBR_SCREENS)
		return serial_write_com1(s);
}

static void	uint32_base_aux(int fd, uint32_t n, const char *base, size_t len)
{
	// TTC - Trust The Caller
	//		base != NULL	-> segfault sur strlen
	//		base != ""		-> division par 0 (DE: Division error, exception cpu)
	// 		len(base) > 1	-> infinite loop 

	if (n >= len)
		uint32_base_aux(fd, n / len, base, len);
	write_char(fd, base[n % len]);
}

void	kfprint_uint32_base(int fd, uint32_t n, const char *base)
{	
	uint32_base_aux(fd, n, base, strlen(base));
}

void	kfprint_uint32(int fd, uint32_t n)
{
	static const char * base = "0123456789";
	kfprint_uint32_base(fd, n, base);
}

void	kfprint_int32(int fd, int32_t n)
{
	static const char * base = "0123456789";
	if (n < 0)
	{
		write_char(fd, '-');
		return kfprint_uint32_base(fd, (uint32_t)-n, base);
	}
	kfprint_uint32_base(fd, (uint32_t)n, base);
}

void kfprint_uint32_padded(int fd, uint32_t n, const char *base, size_t padding)
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
    	write_char(fd, base[0]);	// padding
        padding--;
    }
    kfprint_uint32_base(fd, n, base);
}

static void	map_format(int fd, char c, va_list *ap, int *i)
{
	if (c == 'c')
		return write_char(fd, va_arg(*ap, int));
	if (c == 's')
	{
		char *str = va_arg(*ap, char *);
		if (!str)
			return write(fd, "(null)");
		return write(fd, str);
	}
	if (c == 'p')
	{
		write(fd, "0x");
		return kfprint_uint32_padded(fd, va_arg(*ap, uint32_t), "0123456789abcdef", 8);
	}
	if ((c == 'd') || (c == 'i'))
		return kfprint_int32(fd, va_arg(*ap, int32_t));
	if (c == 'u')
		return kfprint_uint32(fd, va_arg(*ap, uint32_t));
	if (c == 'x')
		return kfprint_uint32_base(fd, va_arg(*ap, uint32_t), "0123456789abcdef");
	if (c == 'X')
		return kfprint_uint32_base(fd, va_arg(*ap, uint32_t), "0123456789ABCDEF");
	if (c == '%')
		return write_char(fd, '%');
	(*i)--;
	write_char(fd, '%');
}

void	kfvprintf(int fd, const char *format, va_list ap)
{
	// TTC - Trust The Caller
	// 		-> fd adresse valide
	// 		-> format adresse valide
	int	i;
	
	i = 0;
	while (format[i])
	{
		if (format[i] == '%')
			map_format(fd, format[++ i], &ap, &i);
		else
			write_char(fd, format[i]);
		i ++;
	}
}

void	kfprintf(int fd, const char *format, ...)
{
	// TTC - Trust The Caller
	// 		-> fd adresse valide
	// 		-> format adresse valide
	va_list	ap;
	
	va_start(ap, format);
	kfvprintf(fd, format, ap);
	va_end(ap);
}
