#ifndef CTYPE_H
#define CTYPE_H

static inline int	isspace(int c) { return ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t') || (c == '\v') || (c == '\f')); }

#endif