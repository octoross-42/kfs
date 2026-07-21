#ifndef SHELL_H
#define SHELL_H

# include "types.h"
# include "vga.h"
# include "ansi.h"
# include "gdt.h"
# include "ctype.h"
# include "debug.h"

# define SHELL_BUFFER_LEN 128

typedef struct
{	
	// position min du cursor dans le shell
	// 			 ↓
	//	 >shell$ cmd truc machin | super
	size_t	col;
	size_t	row;
	size_t	buffer_i;
	char	buffer[SHELL_BUFFER_LEN];
}	shell_t;

# define SHELL_PROMPT "octor-os$ "
# define SHELL_PROMPT_LEN 10

void	init_shells(void);

void	shell_write_char(int screen_nbr, char c);
void	shell_backspace(void);

void	init_shells(void);

void	shell_next_screen(void);
void	shell_prev_screen(void);


#endif