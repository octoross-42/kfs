#ifndef ANSI_H
#define ANSI_H

enum ansi_code
{
	BLACKFG		= 30,
	REDFG  		= 31,
	GREENFG  	= 32,
	YELLOWFG 	= 33,
	BLUEFG   	= 34,
	MAGENTAFG	= 35,
	CYANFG   	= 36,
	LGREYFG  	= 37,

	BBLACKFG	= 90,
	BREDFG  	= 91,
	BGREENFG  	= 92,
	BYELLOWFG 	= 93,
	BBLUEFG   	= 94,
	BMAGENTAFG	= 95,
	BCYANFG   	= 96,
	BLGREYFG  	= 97,

	BLACKBG		= 40,
	REDBG  		= 41,
	GREENBG  	= 42,
	YELLOWBG 	= 43,
	BLUEBG   	= 44,
	MAGENTABG	= 45,
	CYANBG   	= 46,
	LGREYBG  	= 47,

	BBLACKBG	= 100,
	BREDBG  	= 101,
	BGREENBG  	= 102,
	BYELLOWBG 	= 103,
	BBLUEBG   	= 104,
	BMAGENTABG	= 105,
	BCYANBG   	= 106,
	BLGREYBG  	= 107,

	RESET		= 0,
	BRIGHT		= 1,
	DIM			= 2,
	BLINK		= 5,
	REVERSE		= 7,
	BRIGHTOFF	= 22,

	END			= -1
};

# include <stdarg.h>
# include "types.h"
# include "kprint.h"

char	*ansi(enum ansi_code first, ...);

#endif