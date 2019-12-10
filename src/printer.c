#include "system.h"

#define ST_READY 1
#define ST_BUSY 3

#define CMD_ACK 1
#define CMD_PRINTCHR 2

static dtpreg_t *printer0 = (dtpreg_t *)DEV_REG_ADDR(IL_PRINTER, 0);

static int prtr_putchar(char c) {
    unsigned int status = printer0->status;

    if (status != ST_READY)
        return -1;

    printer0->data0 = c;
    printer0->command = CMD_PRINTCHR;

    while ((status = printer0->status) == ST_BUSY)
        ;

    printer0->command = CMD_ACK;

    return 0;
}

void prtr_puts(const char *str) {
    while (*str)
        if (prtr_putchar(*str++))
            return;
}