#include "handler.h"

#include "system.h"
#include "terminal.h"

void syscallHandler(void) {}
void interruptHandler(void) {
    state_t *old = (state_t *)INT_OLDAREA;
#ifdef TARGET_UARM
    PC_SET(old, PC_GET(old) - 4);
#endif
    LDST(old);
}
void trapHandler(void) {}
void tlbExceptionHandler(void) {}