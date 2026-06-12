#ifndef VGA_H
# define VGA_H

# include "io.h"
# include "string.h"
# include <stdarg.h>

// 0x0 = Black    0x8 = Dark Grey
// 0x1 = Blue     0x9 = Light Blue
// 0x2 = Green    0xA = Light Green
// 0x3 = Cyan     0xB = Light Cyan
// 0x4 = Red      0xC = Light Red
// 0x5 = Magenta  0xD = Light Magenta
// 0x6 = Brown    0xE = Yellow
// 0x7 = Grey     0xF = White
enum vga_color
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_YELLOW = 14,
	VGA_COLOR_WHITE = 15,
};

# define VGA_WIDTH		80
# define VGA_HEIGHT		25
# define VGA_BUFFER_ADDRESS	0xB8000

static inline uint16_t	vga_make_entry(char uc, enum vga_color fg, enum vga_color bg)
{
	return (uint16_t)(((bg << 4 | fg) << 8) | uc);
}

static inline uint16_t	vga_get_entry(size_t column, size_t row)
{
	return (((uint16_t *)VGA_BUFFER_ADDRESS)[row * VGA_WIDTH + column]);
}

static inline void	vga_write_entry_at(uint16_t entry, size_t index)
{
	((uint16_t *)VGA_BUFFER_ADDRESS)[index] = entry;
}

void	vga_write(char *str);
void	vga_write_char(char uc);

void	vga_write_uint32(uint32_t n);
void	vga_write_int32(int32_t n);
void	vga_write_uint32_base(uint32_t n, const char *base);
void	vga_write_uint32_padded(uint32_t n, const char *base, size_t padding);
void	printk(const char *format, ...);

void	vga_backspace(void);

void	vga_goto(size_t column, size_t row);

void	vga_change_fg(enum vga_color fg);
void	vga_change_bg(enum vga_color bg);

void	vga_switch_screen(void);
void	vga_init_screens(void);
void	vga_clear_screen(uint16_t *screen);

# define CURSOR_CHAR	0xDB		// bloc plein
// # define CURSOR_CHAR	'_'
void	vga_draw_cursor(void);
void	vga_toggle_cursor(void);
void	vga_reset_cursor(void);
void	vga_tick_cursor(void);

#endif