#ifndef _TERM_H_
#define _TERM_H_

#include "system.h"

#define TERM(line) (termreg_t *)DEV_REG_ADDR(IL_TERMINAL, line)
#define TERM_0 TERM(0)

// Terminal output functions
#define putchar(c) tputchar(TERM_0, c)
int tputchar(termreg_t *term, int c);  // Print character `c` to terminal `term`

#define puts(str) tputs(TERM_0, str)
void tputs(termreg_t *term, const char *str);

// Thanks to ##__VA_ARGS__ we can omit the variadic part
#define print(fmt, ...) tprintf(TERM_0, fmt, ##__VA_ARGS__)
#define println(fmt, ...) print(fmt "\n", ##__VA_ARGS__)
void tprintf(termreg_t *term, const char *fmt, ...);

#define getchar tgetchar(TERM_0)
int tgetchar(termreg_t *term);

#define gets(buf, size) tgets(TERM_0, buf, size)
char *tgets(termreg_t *term, char *buf, int size);

#endif