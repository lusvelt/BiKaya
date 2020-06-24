#ifndef _TERM_H_
#define _TERM_H_

#include "system.h"

#define CMD_TRANSMIT 2
#define CMD_RECEIVE CMD_TRANSMIT

#define CHAR_OFFSET 8
#define TERM_STATUS_MASK 0xFF

#define TERM(line) (termreg_t *)DEV_REG_ADDR(IL_TERMINAL, line)
#define TERM_0 TERM(0)

#define TX_STATUS(term) ((term->transm_status) & TERM_STATUS_MASK)
#define RX_STATUS(term) ((term->recv_status) & TERM_STATUS_MASK)

#define ST_TRANSMITTED 5
#define ST_RECEIVED ST_TRANSMITTED

// Terminal output functions
#define putchar(c) tputchar(TERM_0, c)
int tputchar(termreg_t *term, int c);  // Print character `c` to terminal `term`

#define puts(str) tputs(TERM_0, str)
void tputs(termreg_t *term, const char *str);

// Thanks to ##__VA_ARGS__ we can omit the variadic part
#define print(fmt, ...) tprintf(TERM_0, fmt, ##__VA_ARGS__)
#define println(fmt, ...) print(fmt "\n", ##__VA_ARGS__)
void tprintf(termreg_t *term, const char *fmt, ...);

#ifdef DEBUG
#define DEBUG_TERM TERM(1)
#define debug(fmt, ...) tprintf(DEBUG_TERM, fmt, ##__VA_ARGS__)
#define debugln(fmt, ...) debug(fmt "\n", ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#define debugln(fmt, ...)
#endif

#define getchar tgetchar(TERM_0)
int tgetchar(termreg_t *term);

#define gets(buf, size) tgets(TERM_0, buf, size)
char *tgets(termreg_t *term, char *buf, int size);

#endif