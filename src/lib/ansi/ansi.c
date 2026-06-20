#include "ansi.h"

static	void fill_code(char buf[32], enum ansi_code code, unsigned int *i)
{
	if (code > 9)
		fill_code(buf, code / 10, i);
	buf[(*i) ++] = (code % 10) + '0';
}


char	*ansi(enum ansi_code first, ...)
{
	static char			bufs[8][32];
	static int			rotation = 0;

	va_list			args;
	unsigned int	i;
	enum ansi_code	code;
	
	char *buf = bufs[rotation];
	rotation = (rotation + 1) % 8;

	if (first < 0)
		return ((char *)0);
	buf[0] = '\033';
	buf[1] = '[';
	i = 2;

	fill_code(buf, first, &i);

	va_start(args, first);
	code = va_arg(args, enum ansi_code);
	
	while (code >= 0)
	{
		buf[i ++] = ';';
		fill_code(buf, code, &i);

		code = va_arg(args, enum ansi_code);
	}
	
	buf[i ++] = 'm';
	buf[i] = '\0';
	va_end(args);
	
	return (buf);
}
