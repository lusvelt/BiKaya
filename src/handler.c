#include "handler.h"

#include "scheduler.h"
#include "syscall.h"
#include "system.h"
#include "utils.h"

void syscallHandler(void) {
    pcb_t *current = getCurrent();
    state_t *old = (state_t *)SYSBK_OLDAREA;
    uint32_t cause = CAUSE_GET(old);

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
                SYSCALL_RETURN(old, createProcess(state, priority, cpid));
                LDST(old);
                break;
            case TERMINATEPROCESS:
                pcb_t *pid = REG_GET(old, a1);
                SYSCALL_RETURN(old, terminateProcess(pid));
                if (pid)
                    LDST(old);  // No need to go through scheduler since process is using its time slice fairly
                else
                    start();
                break;
            case VERHOGEN:
                int *semaddr = REG_GET(old, a1);
                verhogen(semaddr);
                LDST(old);
                break;
            case PASSEREN:
                int *semaddr = REG_GET(old, a1);
                int blocked = passeren(semaddr, current);
                if (blocked)
                    start();
                else
                    LDST(old);
                break;
            case WAITIO:
                uint32_t command = REG_GET(old, a1);
                uint32_t *reg = REG_GET(old, a2);
                bool subdev = REG_GET(old, a3);
                waitIo(command, reg, subdev);
                start();
                break;
            case SPECPASSUP:
                spu_t type = REG_GET(old, a1);
                state_t *spuOld = REG_GET(old, a2);
                state_t *spuNew = REG_GET(old, a3);
                SYSCALL_RETURN(old, specPassUp(type, spuOld, spuNew));
                LDST(old);
                break;
            case GETPID:
                memaddr pid = REG_GET(old, a1);
                memaddr ppid = REG_GET(old, a2);
                getPid(current, pid, ppid);
                break;
            default:
                if (current->sysbk_new == NULL) {
                    terminateProcess(current);
                    next(old);
                } else {  // Unnecessary but increases readibility
                    current->sysbk_old = old;
                    LDST(current->sysbk_new);
                }
        }
    }
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
    else {
        // gettare la bitmap per vedere su quale device della linea c'è l'interrupt pending
        int *semkey = getSemKey(cause);
        // gettare il processo in testa alla coda dei bloccati sul semaforo
        // settare il valore di ritorno della syscall waitio nel pcb
        // restituire il controllo allo scheduler
    }

    LDST(old);
}

void trapHandler(void) {
    pcb_t *current = getCurrent();
    state_t *old = (state_t *)PGMTRAP_OLDAREA;

    if (current->trap_new == NULL) {
        terminateProcess(current);
        next(old);
    } else {
        current->trap_old = old;
        LDST(current->trap_new);
    }
}

void tlbExceptionHandler(void) {
    pcb_t *current = getCurrent();
    state_t *old = (state_t *)TLB_OLDAREA;

    if (current->tlb_new == NULL) {
        terminateProcess(current);
        next(old);
    } else {
        current->tlb_old = old;
        LDST(current->tlb_new);
    }
}