#include "mm.h"

void set_used(uint32_t n) { bitmap[n >> 5] |=  (1u << (n & 31)); }
void set_free(uint32_t n) { bitmap[n >> 5] &= ~(1u << (n & 31)); }
int  is_used (uint32_t n) { return bitmap[n >> 5] &   (1u << (n & 31)); }