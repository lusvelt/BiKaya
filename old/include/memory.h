#ifndef _MEMSET_H_
#define _MEMSET_H_

#include <stddef.h>

void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t len);

#endif