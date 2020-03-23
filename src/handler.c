#include "handler.h"

#include "system.h"
#include "term.h"

void syscallHandler(void) {}

void interruptHandler(void) {
    state_t *oldState = (state_t *)INT_OLDAREA;
    oldState->pc -= 4;
    LDST(oldState);
}

void trapHandler(void) {}
void tlbExceptionHandler(void) {}