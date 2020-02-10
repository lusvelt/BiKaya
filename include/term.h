#ifndef __TERM_H__
#define __TERM_H__

#include "system.h"

#define TERM(line) (termreg_t *)DEV_REG_ADDR(IL_TERMINAL, line)
#define TERM_0 TERM(0)

// Term I/O functions

int tputchar(termreg_t *term, int c);  // Print character `c` to terminal `term`
int putchar(int c);                    // Print character `c` to terminal 0

void tputs(termreg_t *term, const char *str);
void puts(const char *str);

void tprintf(termreg_t *term, const char *fmt, ...);
void printf(const char *fmt, ...);

int tgetchar(termreg_t *term);
int getchar(void);

char *tgets(termreg_t *term, char *buf, int size);
char *gets(char *buf, int size);

#endif