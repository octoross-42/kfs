#ifndef PRINTK_H
#define PRINTK_H

# include "types.h"
# include <stdarg.h>
# include "string.h"

# include "serial.h"

void	kfprint_uint32_base(void (*write_char)(char), uint32_t n, const char *base);
void	kfprint_uint32(void (*write_char)(char), uint32_t n);
void	kfprint_int32(void (*write_char)(char), int32_t n);
void	kfprint_uint32_padded(void (*write_char)(char), uint32_t n, const char *base, size_t padding);

void	kfvprintf(void (*write_char)(char), const char *format, va_list ap);
void	kfprintf(void (*write_char)(char), const char *format, ...);

void	printk(const char *format, ...);

#endif