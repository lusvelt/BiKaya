#include "handler.h"

#include "scheduler.h"
#include "system.h"
#include "utils.h"

void syscallHandler(void) {
    state_t *old = (state_t *)SYSBK_OLDAREA;
    uint32_t cause = CAUSE_GET(old);

    if (CAUSE_IS_SYSCALL(cause)) {
#ifdef TARGET_UMPS
        PC_SET(old, PC_GET(old) - WORD_SIZE);
#endif

        uint32_t no = REG_GET(old, a0);
        // Probably needed for phase 2
        // uint32_t arg1 = REG_GET(old, a1);
        // uint32_t arg2 = REG_GET(old, a2);
        // uint32_t arg3 = REG_GET(old, a3);

        switch (no) {
            case TERMINATE_PROCESS:
                if (killCurrent() == OK)
                    start();
                else
                    EXIT("Failed to kill process: ready queue empty");
                break;
            default:
                EXIT("Unknown System Call no: %d", no);
        }
    }
}

void interruptHandler(void) {
    state_t *old = (state_t *)INT_OLDAREA;
#ifdef TARGET_UARM
    PC_SET(old, PC_GET(old) - WORD_SIZE);
#endif
    uint32_t cause = CAUSE_GET(old);

    if (IS_TIMER_INT(cause))
        next(old);

    LDST(old);
}

void trapHandler(void) {
}

void tlbExceptionHandler(void) {
}