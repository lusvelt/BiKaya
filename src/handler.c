#include "handler.h"

#include "scheduler.h"
#include "syscall.h"
#include "system.h"
#include "utils.h"

HIDDEN inline time_t getTOD() {
    return (getTODHI() << 32) | getTODLO());
}

time_t kernel_enter_tm;

HIDDEN void setTimeEnterKernel(pcb_t *current) {
    kernel_enter_tm = getTOD();
    current->kernel_tm = getTOD();
    current->user_tm =
}
HIDDEN void setTimeExitKernel(pcb_t *current) {
    current->kernel_tm = current->kernel_tm + (getTOD() - kernel_enter_tm);
}

void syscallHandler(void) {
    pcb_t *current = getCurrent();
    state_t *old = (state_t *)SYSBK_OLDAREA;
    uint32_t cause = CAUSE_GET(old);
    setTimeEnterKernel(current);

    if (CAUSE_IS_SYSCALL(cause)) {
#ifdef TARGET_UMPS
        PC_SET(old, PC_GET(old) + WORD_SIZE);
#endif

        uint32_t no = REG_GET(old, a0);

        switch (no) {
            case GETCPUTIME:
                uint32_t *arg1 = REG_GET(old, a1);
                uint32_t *arg2 = REG_GET(old, a2);
                uint32_t *arg3 = REG_GET(old, a3);

                *arg1 = current->user_tm;
                *arg2 = current->kernel_tm;
                *arg3 = getTOD() - current->start_tm;

                break;
            case CREATEPROCESS:
                state_t *state = REG_GET(old, a1);
                int priority = REG_GET(old, a2);
                void **cpid = REG_GET(old, a3);
                SYSCALL_RETURN(state, createProcess(state, priority, cpid));
                break;
            case TERMINATEPROCESS:
                pcb_t *pid = REG_GET(old, a1);
                SYSCALL_RETURN(state, terminateProcess(pid));
                break;
            case VERHOGEN:
                int *semaddr = REG_GET(old, a1);
                verhogen(semaddr);
                break;
            case PASSEREN:
                int *semaddr = REG_GET(old, a1);
                passeren(semaddr);
                break;
            case WAITIO:
                break;
            case SPECPASSUP:
                break;
            case GETPID:
                break;

            default:
                //pass to custom handler if present
                EXIT("Unknown System Call no: %d", no);
        }
    }
    setTimeExitKernel(current);
}

// TODO: remember to set enter_kernel
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