#include "handler.h"

#include "system.h"
#include "term.h"

void syscallHandler(void) {}
void interruptHandler(void) {
    state_t *old = INT_OLDAREA;
    ((termreg_t *)TERM_0)->transm_command = 1;
    old->pc -= 4;
    LDST(old);
}
void trapHandler(void) {}
void tlbExceptionHandler(void) {}