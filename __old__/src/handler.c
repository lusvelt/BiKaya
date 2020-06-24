#include "handler.h"

#include "asl.h"
#include "scheduler.h"
#include "syscall.h"
#include "system.h"
#include "terminal.h"

void nop() {}

// this macro sets state to state found in old, to avoid
// discrepancies between last known state and real state
// only internal use
#define ENTER_HANDLER(oldarea)         \
    pcb_t *current = getCurrent();     \
    state_t *old = (state_t *)oldarea; \
    nop();                             \
    current->p_s = *old;               \
    old = &current->p_s

HIDDEN int devFromBitmap(uint8_t bitmap) {
    switch (bitmap) {
        case 0b00000001:
            return 0;
        case 0b00000010:
            return 1;
        case 0b00000100:
            return 2;
        case 0b00001000:
            return 3;
        case 0b00010000:
            return 4;
        case 0b00100000:
            return 5;
        case 0b01000000:
            return 6;
        case 0b10000000:
            return 7;
    }
}

#define TRACE_SYSCALL(fmt, ...)                                 \
    {                                                           \
        bool toReset = !(INT_IS_PENDING(getCAUSE(), IL_TIMER)); \
        uint32_t time = getTIMER();                             \
        debugln(fmt, ##__VA_ARGS__);                            \
        printReadyQueue();                                      \
        if (toReset)                                            \
            setTIMER(time);                                     \
    }

void handler_syscall(void) {
    ENTER_HANDLER(SYSBK_OLDAREA);
    uint32_t cause = CAUSE_GET(old);

    if (CAUSE_IS_SYSCALL(cause)) {
#ifdef TARGET_UMPS
        PC_SET(old, PC_GET(old) + WORD_SIZE);
#endif

        uint32_t no = REG_GET(old, A0);

        switch (no) {
            case GETCPUTIME: {
                uint32_t *arg1 = REG_GET(old, A1);
                uint32_t *arg2 = REG_GET(old, A2);
                uint32_t *arg3 = REG_GET(old, A3);

                *arg1 = 1;
                *arg2 = 2;
                *arg3 = 3;

                LDST(old);
                break;
            }
            case CREATEPROCESS: {
                state_t *state = REG_GET(old, A1);
                int priority = REG_GET(old, A2);
                void **cpid = REG_GET(old, A3);
                SYSCALL_RETURN(old, syscall_create_process(state, priority, cpid));
                if (current == getCurrent())
                    LDST(old);
                else
                    start();
                break;
            }
            case TERMINATEPROCESS: {
                pcb_t *pid = REG_GET(old, A1);
                SYSCALL_RETURN(old, syscall_terminate_process(pid));
                if (pid)
                    LDST(old);  // No need to go through scheduler since process is using its time slice fairly
                else
                    start();
                break;
            }
            case VERHOGEN: {
                int *semaddr = REG_GET(old, A1);
                syscall_verhogen(semaddr);
                if (current == getCurrent())
                    LDST(old);
                else
                    start();
                break;
            }
            case PASSEREN: {
                int *semaddr = REG_GET(old, A1);
                int blocked = syscall_passeren(semaddr, current);
                if (blocked)
                    start();
                else
                    LDST(old);
                break;
            }
            case WAITIO: {
                uint32_t command = REG_GET(old, A1);
                uint32_t *reg = REG_GET(old, A2);
                bool subdev = REG_GET(old, A3);
                syscall_waitio(command, reg, subdev);
                start();
                break;
            }
            case SPECPASSUP: {
                spu_t type = REG_GET(old, A1);
                state_t *spuOld = REG_GET(old, A2);
                state_t *spuNew = REG_GET(old, A3);
                SYSCALL_RETURN(old, syscall_specpassup(type, spuOld, spuNew));
                if (current == getCurrent())
                    LDST(old);
                else
                    start();
                break;
            }
            case GETPID: {
                uint32_t *pid = REG_GET(old, A1);
                uint32_t *ppid = REG_GET(old, A2);
                syscall_getpid(current, pid, ppid);
                LDST(old);
                break;
            }
            default:
                debugln("Entro nel default con %p", current->p_s.lr);
                debugln("&pcb: %p", current);
                debugln("sysbk_new: %p", current->sysbk_new);
                debugln("sysbk_old: %p", current->sysbk_old);
                if (current->sysbk_new == NULL) {
                    syscall_terminate_process(NULL);
                    start();
                } else {  // Unnecessary but increases readibility
                    debugln("Call custom syscall");
                    *(current->sysbk_old) = *old;
                    debugln("updated sysbk_old: %p (lr = %p)", current->sysbk_old, current->sysbk_old->lr);
                    LDST(current->sysbk_new);
                }
        }
    }
}

HIDDEN void handleInterrupt(uint8_t line) {
    uint8_t *bitmap = CDEV_BITMAP_ADDR(line);
    int dev = devFromBitmap(*bitmap);
    devreg_t *reg = DEV_REG_ADDR(line, dev);
    int *semKey = syscall_get_device_sem_key(reg);
    pcb_t *p = asl_remove_blocked(semKey);

    addToReadyQueue(p);
    *semKey = 1;  // FIXME: è giusto aggiornare manualmente il valore del semaforo?
    uint32_t status;
    if (line != IL_TERMINAL) {
        dtpreg_t *dev = &reg->dtp;
        status = dev->status;
        dev->command = CMD_ACK;
    } else {
        termreg_t *term = &reg->term;
        if (TX_STATUS(term) == ST_READY)  // Ricezione
            status = term->recv_status;
        else if (RX_STATUS(term) == ST_READY)  // Trasmissione
            status = term->transm_status;
        term->recv_command = CMD_ACK;
        term->transm_command = CMD_ACK;
    }

    SYSCALL_RETURN(&p->p_s, status);
}

// TODO: remember to set enter_kernel
void handler_interrupt(void) {
    ENTER_HANDLER(INT_OLDAREA);
#ifdef TARGET_UARM
    PC_SET(old, PC_GET(old) - WORD_SIZE);
#endif
    uint32_t cause = CAUSE_GET(old);

    if (INT_IS_PENDING(cause, IL_TIMER)) {
        // remove the current process
        removeHeadFromReadyQueue();
        aging();
        // We reset current process priority to avoid inflated priority
        current->priority = current->original_priority;
        addToReadyQueue(current);
        SET_TIMER(TIME_SLICE);
    }
    if (INT_IS_PENDING(cause, IL_DISK))
        handleInterrupt(IL_DISK);
    if (INT_IS_PENDING(cause, IL_TAPE))
        handleInterrupt(IL_TAPE);
    if (INT_IS_PENDING(cause, IL_ETHERNET))
        handleInterrupt(IL_ETHERNET);
    if (INT_IS_PENDING(cause, IL_PRINTER))
        handleInterrupt(IL_ETHERNET);
    if (INT_IS_PENDING(cause, IL_TERMINAL))
        handleInterrupt(IL_TERMINAL);

    // printReadyQueue();
    // if head didn't change... peculiar pattern. to be investigated.
    // we may want to include this check in start
    if (current == getCurrent())
        LDST(old);
    else
        start();
}

void handler_trap(void) {
    ENTER_HANDLER(PGMTRAP_OLDAREA);

    if (current->trap_new == NULL) {
        syscall_terminate_process(current);
        start();
    } else {
        *(current->trap_old) = *old;
        LDST(current->trap_new);
    }
}

void handler_tlb_exception(void) {
    ENTER_HANDLER(TLB_OLDAREA);

    if (current->tlb_new == NULL) {
        syscall_terminate_process(current);
        start();
    } else {
        *(current->tlb_old) = *old;
        LDST(current->tlb_new);
    }
}