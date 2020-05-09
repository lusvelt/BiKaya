#include "terminal.h"

#include <stdarg.h>
#include <stdint.h>

#include "const.h"
#include "system.h"

int tputchar(termreg_t *term, int c) {
    uint32_t stat;

    stat = TX_STATUS(term);
    if (stat != ST_READY && stat != ST_TRANSMITTED)
        return -1;

    term->transm_command = ((c << CHAR_OFFSET) | CMD_TRANSMIT);

    while ((stat = TX_STATUS(term)) == ST_BUSY)
        ;

    term->transm_command = CMD_ACK;

    if (stat != ST_TRANSMITTED)
        return -1;
    else
        return 0;
}

void tputs(termreg_t *term, const char *str) {
    while (*str)
        if (tputchar(term, *str++))
            return;
}

HIDDEN char *convert(uint32_t num, int base) {
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
                    tputs(term, convert((uint32_t)va_arg(args, void *), 16));
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

/**
 * It is roughly the same as 'term_putchar' except that the character
 * received from the terminal is stored into the status field of the
 * receiver side of the terminal.
 */
int tgetchar(termreg_t *term) {
    uint32_t stat;

    stat = RX_STATUS(term);
    if (stat != ST_READY && stat != ST_RECEIVED)
        return -1;

    term->recv_command = CMD_RECEIVE;

    while ((stat = RX_STATUS(term)) == ST_BUSY)
        ;

    char c = term->recv_status >> CHAR_OFFSET;
    term->recv_command = CMD_ACK;

    if (stat != ST_RECEIVED)
        return -1;
    else
        return c;
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
