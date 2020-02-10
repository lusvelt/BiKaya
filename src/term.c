#include "term.h"

#include <stdarg.h>
#include "const.h"
#include "system.h"

#define ST_READY 1
#define ST_BUSY 3
#define ST_TRANSMITTED 5
#define ST_RECEIVED ST_TRANSMITTED

#define CMD_ACK 1
#define CMD_TRANSMIT 2
#define CMD_RECEIVE CMD_TRANSMIT

#define CHAR_OFFSET 8
#define TERM_STATUS_MASK 0xFF

#define tx_status(term) ((term->transm_status) & TERM_STATUS_MASK)
#define rx_status(term) ((term->recv_status) & TERM_STATUS_MASK)

HIDDEN termreg_t *term0 = TERM(0);

int tputchar(termreg_t *term, int c) {
    unsigned int stat;

    stat = tx_status(term);
    if (stat != ST_READY && stat != ST_TRANSMITTED)
        return -1;

    term->transm_command = ((c << CHAR_OFFSET) | CMD_TRANSMIT);

    while ((stat = tx_status(term)) == ST_BUSY)
        ;

    term->transm_command = CMD_ACK;

    if (stat != ST_TRANSMITTED)
        return -1;
    else
        return 0;
}

int putchar(int c) {
    return tputchar(TERM_0, c);
}

void tputs(termreg_t *term, const char *str) {
    while (*str)
        if (tputchar(term, *str++))
            return;
}

void puts(const char *str) {
    tputs(TERM_0, str);
}

HIDDEN char *convert(unsigned int num, int base) {
    const char digits[] = "0123456789ABCDEF";
    static char buffer[64];
    static char *ptr;

    ptr = &buffer[63];
    *ptr = '\0';

    do {
        *--ptr = digits[num % base];
        num /= base;
    } while (num != 0);

    return ptr;
}

HIDDEN void vtprintf(termreg_t *term, const char *fmt, va_list args) {
    for (; *fmt; fmt++) {
        if (*fmt == '%') {
            switch (*(++fmt)) {
                case 'c':
                    tputchar(term, va_arg(args, int));
                    break;
                case 'd':
                    tputs(term, convert(va_arg(args, int), 10));
                    break;
                case 's':
                    tputs(term, va_arg(args, char *));
                    break;
                case 'p':
                    tputs(term, "0x");
                    tputs(term, convert(va_arg(args, void *), 16));
                    break;
            }
        } else {
            tputchar(term, *fmt);
        }
    }
}

void tprintf(termreg_t *term, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vtprintf(term, fmt, args);
    va_end(args);
}

void printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vtprintf(TERM_0, fmt, args);
    va_end(args);
}

/**
 * It is roughly the same as 'term_putchar' except that the character
 * received from the terminal is stored into the status field of the
 * receiver side of the terminal.
 */
int tgetchar(termreg_t *term) {
    unsigned int stat;

    stat = rx_status(term);
    if (stat != ST_READY && stat != ST_RECEIVED)
        return -1;

    term->recv_command = CMD_RECEIVE;

    while ((stat = rx_status(term)) == ST_BUSY)
        ;

    char c = term->recv_status >> CHAR_OFFSET;
    term->recv_command = CMD_ACK;

    if (stat != ST_RECEIVED)
        return -1;
    else
        return c;
}

int getchar(void) {
    return tgetchar(TERM_0);
}

/** 
 * It reads a string from terminal0 until it founds a new line ('\n')
 * or the number of characters read is equal to size-1 and stores them
 * in a buffer passed as argument by the caller. If new line
 * is not found during first call to the function then successive calls
 * cause term_gets to read more charcters until it eventually gets a
 * new line (it informs the caller when this event happen by returning
 * a NULL pointer instead of a pointer to the buffer passed in).
 * It's also important to note that the buffer passed to the function
 * is always null-terminated.
 */
char *tgets(termreg_t *term, char *buf, int size) {
    int i, len = size - 1;

    for (i = 0; i < len; i++) {
        char c = tgetchar(term);
        buf[i] = c;

        if (c == '\n') {
            buf[i + 1] = '\0';
            return NULL;
        }
    }

    buf[i] = '\0';
    return buf;
}

char *gets(char *buf, int size) {
    return tgets(TERM_0, buf, size);
}
