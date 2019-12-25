#include "printer.h"
#include "system.h"
#include "term.h"

#define BUFSIZE 10

int main(void) {
    char str[BUFSIZE], *result;

    do {
        result = term_gets(str, BUFSIZE);
        term_puts(str);
        prtr_puts(str);
    } while (result != NULL);

    /* Go to sleep indefinetely */
    while (1)
        WAIT();
    return 0;
}
