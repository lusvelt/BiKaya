#include "printer.h"

#include "const.h"
#include "types.h"

#define ST_READY 1
#define ST_BUSY 3

#define CMD_ACK 1
#define CMD_PRINTCHR 2

HIDDEN dtpreg_t *printer0 = (dtpreg_t *)DEV_REG_ADDR(IL_PRINTER, 0);

/**
 * It writes a character to printer0 by sending a CMD_PRINTCHAR
 * to it (the printer).
 */
HIDDEN int prtr_putchar(char c) {
    unsigned int status = printer0->status;

    if (status != ST_READY) /* Check printer readiness */
        return -1;

    printer0->data0 = c;
    printer0->command = CMD_PRINTCHR;

    while ((status = printer0->status) == ST_BUSY) /* Busy-waiting until command completion */
        ;

    printer0->command = CMD_ACK;

    return 0;
}

/** 
 * It prints the string passed in one character at a time onto
 * printer0.
 */
void prtr_puts(const char *str) {
    while (*str)
        if (prtr_putchar(*str++))
            return;
}