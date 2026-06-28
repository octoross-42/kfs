#ifndef PRINTK_H
#define PRINTK_H

# include "types.h"
# include <stdarg.h>
# include "string.h"

# include "serial.h"

void	kfprint_uint32_base(int fd, uint32_t n, const char *base);
void	kfprint_uint32(int fd, uint32_t n);
void	kfprint_int32(int fd, int32_t n);
void	kfprint_uint32_padded(int fd, uint32_t n, const char *base, size_t padding);

void	kfvprintf(int fd, const char *format, va_list ap);
void	kfprintf(int fd, const char *format, ...);

void	printk(const char *format, ...);

# include "vga.h" 

#endif