#ifndef __MEMSET_H__
#define __MEMSET_H__

typedef unsigned int size_t;

void *memset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    while (n--)
        *p++ = (unsigned char)c;
    return s;
}

#endif