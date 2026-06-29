#include "shell.h"

shell_t			shells[NBR_SCREENS];
shell_t			*active_shell = &shells[0];

void	shell_write_char(char c, int screen_nbr)
{
	shell_t	*shell;

	if (c == '\n')
		
	if (screen_nbr < 0)
		shell = active_shell;
	else
		shell = &shells[screen_nbr];
	if (shell->buffer_i >= SHELL_BUFFER_LEN)
		return ;
	shell->buffer[shell->buffer_i ++] = c;
	vga_write_char(c, screen_nbr);
}

void	shell_backspace(void)
{
	if (vga_backspace_shell() && (active_shell->buffer_i > 0))
		active_shell->buffer[--(active_shell->buffer_i)] = 0;
}


void	shell_prompt(unsigned int screen_nbr)
{
	enum vga_color	fg;
	enum vga_color	bg;

	fg = vga_get_fg(screen_nbr);
	bg = vga_get_bg(screen_nbr);
	vga_change_fg(VGA_COLOR_CYAN, screen_nbr);
	vga_change_bg(VGA_COLOR_BLACK, screen_nbr);
	vga_write(SHELL_PROMPT, screen_nbr);
	vga_change_fg(fg, screen_nbr);
	vga_change_bg(bg, screen_nbr);
}

static char	*utf8_to_cp437(char *s)
{
	unsigned int i;
	unsigned int j;

	i = 0;
	j = 0;
	while (s[i])
	{
		if ((unsigned char)(s[i]) == 0xE2)
		{
			if ((unsigned char)(s[i + 1]) == 0x96)
			{
				if ((unsigned char)(s[i + 2]) == 0x80)		s[j] = 0xDF;		// '▀'
				else if ((unsigned char)(s[i + 2]) == 0x84)	s[j] = 0xDC;		// '▄'
				else if ((unsigned char)(s[i + 2]) == 0x88)	s[j] = 0xDB;		// '█'
				// '▌' -> 0xDD
				// '▐' -> 0xDE
			}
			else if (((unsigned char)(s[i + 1]) == 0x94) && ((unsigned char)(s[i + 2]) == 0x80))
				s[j] = 0xC4;	// '─'
			
			i += 3;
		}
		
		else
			s[j] = s[i ++];
		
		j ++;
	}
	s[j] = '\0';
	return (s);
}

static void	init_shell_screen(size_t screen_nbr)
{
	size_t		i;
	
	shells[screen_nbr].buffer_i = 0;
	vga_init_screen(screen_nbr);
	i = 0;
	while (i < SHELL_BUFFER_LEN)
		shells[screen_nbr].buffer[i ++] = '\0';

	kfprintf(screen_nbr, utf8_to_cp437("\n        %s         ▄▄           ▄▀█\n"), ansi(LGREYFG, END));
	kfprintf(screen_nbr, utf8_to_cp437("        ▄▀▀▀▄  ▄▀ █           ▀▄▀ ▄▄  \n"));
	kfprintf(screen_nbr, utf8_to_cp437("        ▀▄   █▄▀▄▄▀       ▄▀▀▄  ▄▀ █ \n"));
	kfprintf(screen_nbr, utf8_to_cp437("          ▀▀▄▄█▀▄         ▀▄▄ █ █ █  \n"));
	kfprintf(screen_nbr, utf8_to_cp437("              █  ▀▄▄▄        ▀█ █▀    \n"));
	kfprintf(screen_nbr, utf8_to_cp437("              █      ▀▀▀▀▀▀▄▄▄▄▄▄▄▄▄▄▄▄ \n"));
	kfprintf(screen_nbr, utf8_to_cp437("              █                        ▀▄\n"));
	kfprintf(screen_nbr, utf8_to_cp437("               █                        █\n"));
	kfprintf(screen_nbr, utf8_to_cp437("                █              ▄▄       █\n"));
	kfprintf(screen_nbr, utf8_to_cp437("                 ▀▀▄▄          %s▀%s▀      ▄▀\n"), ansi(CYANFG, END), ansi(LGREYFG, END));
	kfprintf(screen_nbr, utf8_to_cp437("                 ▄▀  ▄▀▄▄▄          ▄▄▄▀\n"));
	kfprintf(screen_nbr, utf8_to_cp437("                  ▀▀▀     ▀▀▀▀▀▀▀▀▀▀\n\n\n"));
	kfprintf(screen_nbr, utf8_to_cp437("                 █ ▄▄▀▀  █▀▀▀▀▀ ▄▀▀▀▀▀\n"));
	kfprintf(screen_nbr, utf8_to_cp437("                 █▀  ▀▄  █▀▀▀▀   ▀▀▀▀▄\n"));
	kfprintf(screen_nbr, utf8_to_cp437("                 ▀    ▀  ▀      ▀▀▀▀▀▀\n"));
	kfprintf(screen_nbr, utf8_to_cp437("    ──────────────────────────────────────────────────\n"));
	kfprintf(screen_nbr, utf8_to_cp437("    %sBy: ebouvier                          By: octoross%s\n\n"),
		ansi(YELLOWFG, END), ansi(RESET, END));

	shell_prompt(screen_nbr);
	
	shells[screen_nbr].col = vga_get_col(screen_nbr);
	shells[screen_nbr].row = vga_get_row(screen_nbr);
}

void	init_shells(void)
{
	unsigned int i;

	i = 0;
	while (i < NBR_SCREENS)
		init_shell_screen(i ++);
}
