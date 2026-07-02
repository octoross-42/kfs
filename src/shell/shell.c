#include "shell.h"

shell_t			shells[NBR_SCREENS];
shell_t			*active_shell = &shells[0];

// static shell_t	*get_shell(int screen_nbr)
// {
// 	if (screen_nbr < 0)
// 		return (active_shell);
// 	return (&shells[screen_nbr]);
// }

static void	shell_prompt(shell_t *shell)
{
	enum vga_color	fg;
	enum vga_color	bg;
	unsigned int	screen_nbr;

	screen_nbr = shell - shells;
	fg = vga_get_fg(screen_nbr);
	bg = vga_get_bg(screen_nbr);
	vga_change_fg(screen_nbr, VGA_COLOR_CYAN);
	vga_change_bg(screen_nbr, VGA_COLOR_BLACK);
	
	vga_write(screen_nbr, SHELL_PROMPT);
	
	shell->col = vga_get_col(screen_nbr);
	shell->row = vga_get_row(screen_nbr);
	
	vga_change_fg(screen_nbr, fg);
	vga_change_bg(screen_nbr, bg);
}

static inline int	contains_cmd(char *str, char *cmd, size_t cmd_len)
{
	return (!strncmp(str, cmd, cmd_len) && ((str[cmd_len] == '\0') || isspace(str[cmd_len])));
}

static void	isolate_first_word(char *s)
{
	while (*s)
	{
		if (isspace(*s))
		{
			*s = '\0';
			return ;
		}
		s ++;
	}
}

static void	shell_no_heap_cmd(shell_t *shell)
{
	char			*cmd_buffer;

	cmd_buffer = shell->buffer;
	while (*cmd_buffer == ' ')
		cmd_buffer ++;
	if (!*cmd_buffer)
		return ;
	if (contains_cmd(cmd_buffer, "printks", 7))
	{
		print_kernel_stack(shell - shells);
		print_kernel_stack(NBR_SCREENS);
	}
	else
	{
		isolate_first_word(cmd_buffer);
		kfprintf(shell - shells, "%s: command not found\n", cmd_buffer);
	}
}

void	shell_enter_cmd(shell_t *shell)
{
	size_t			i;

	if (!*(shell->buffer))
		return shell_prompt(shell);
	shell_no_heap_cmd(shell);

	i = 0;
	while (i < shell->buffer_i)
		shell->buffer[i ++] = '\0';
	shell->buffer_i = 0;
	shell_prompt(shell);
}

void	shell_write_char(int screen_nbr, char c)
{
	shell_t	*shell;

	if (screen_nbr < 0)
		shell = active_shell;
	else
		shell = &shells[screen_nbr];
	
	if ((shell->buffer_i > 0) && (shell->buffer[shell->buffer_i - 1] == '\\') && (c == '\n'))
	{
		shell->buffer[shell->buffer_i - 1] = '\n';
		return vga_write_char(screen_nbr, '\n');;
	}

	if (c == '\n')
	{
		vga_write_char(screen_nbr, c);
		return shell_enter_cmd(shell);
	}
	if (shell->buffer_i >= SHELL_BUFFER_LEN)
		return ;

	shell->buffer[shell->buffer_i ++] = c;
	vga_write_char(screen_nbr, c);
}

void	shell_backspace(void)
{
	if (vga_backspace_shell() && (active_shell->buffer_i > 0))
		active_shell->buffer[--(active_shell->buffer_i)] = 0;
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

	shell_prompt(&shells[screen_nbr]);
}

void	init_shells(void)
{
	unsigned int i;

	i = 0;
	while (i < NBR_SCREENS)
		init_shell_screen(i ++);
}

void	shell_next_screen(void)
{
	if (active_shell == shells + NBR_SCREENS - 1)
		active_shell = shells;
	else
		active_shell ++;

	vga_next_screen();
}

void	shell_prev_screen(void)
{
	if (active_shell == shells)
		active_shell = shells + NBR_SCREENS - 1;
	else
		active_shell --;

	vga_prev_screen();
}
