#ifndef SHELL_H
#define SHELL_H

# include "types.h"
# include "vga.h"
# include "ansi.h"

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

void	shell_write_char(char c, int screen_nbr);
void	shell_backspace(void);

void	shell_prompt(unsigned int screen_nbr);

void	init_shells(void);

#endif