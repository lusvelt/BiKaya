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

static termreg_t *term0_reg = (termreg_t *)DEV_REG_ADDR(IL_TERMINAL, 0);

static unsigned int tx_status(termreg_t *tp) {
    return ((tp->transm_status) & TERM_STATUS_MASK);
}

static unsigned int rx_status(termreg_t *tp) {
    return ((tp->recv_status) & TERM_STATUS_MASK);
}

static int term_putchar(char c) {
    unsigned int stat;

    stat = tx_status(term0_reg);
    if (stat != ST_READY && stat != ST_TRANSMITTED)
        return -1;

    term0_reg->transm_command = ((c << CHAR_OFFSET) | CMD_TRANSMIT);

    while ((stat = tx_status(term0_reg)) == ST_BUSY)
        ;

    term0_reg->transm_command = CMD_ACK;

    if (stat != ST_TRANSMITTED)
        return -1;
    else
        return 0;
}

// It is roughly the same as 'term_putchar' expect that the character
// received from the terminal is stored into the status field of the
// receiver side of the terminal.
static char term_getchar() {
    unsigned int stat;

    stat = rx_status(term0_reg);
    if (stat != ST_READY && stat != ST_RECEIVED)
        return -1;

    term0_reg->recv_command = CMD_RECEIVE;

    while ((stat = rx_status(term0_reg)) == ST_BUSY)
        ;

    char c = term0_reg->recv_status >> CHAR_OFFSET;
    term0_reg->recv_command = CMD_ACK;

    if (stat != ST_RECEIVED)
        return -1;
    else
        return c;
}

void term_puts(const char *str) {
    while (*str)
        if (term_putchar(*str++))
            return;
}

// It reads a string from terminal0 until it founds a new line ('\n')
// or the number of characters read is equal to size-1 and stores them
// in a buffer passed as argument by the caller. If new line
// is not found during first call to the function then successive calls
// cause term_gets to read more charcters until it eventually gets a
// new line (it informs the caller when this event happen by returning
// a NULL pointer instead of a pointer to the buffer passed in).
// It's also important to note that the buffer passed to the function
// is always null-terminated.
char *term_gets(char *buf, int size) {
    int i, len = size - 1;

    for (i = 0; i < len; i++) {
        char c = term_getchar();
        buf[i] = c;

        if (c == '\n') {
            buf[i + 1] = '\0';
            return NULL;
        }
    }

    buf[i + 1] = '\0';
    return buf;
}
