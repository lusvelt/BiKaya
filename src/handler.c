#include "handler.h"

#include "asl.h"
#include "scheduler.h"
#include "syscall.h"
#include "system.h"
#include "terminal.h"

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

void syscallHandler(void) {
    pcb_t *current = getCurrent();
    state_t *old = (state_t *)SYSBK_OLDAREA;
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
                SYSCALL_RETURN(old, createProcess(state, priority, cpid));
                LDST(old);
                break;
            }
            case TERMINATEPROCESS: {
                pcb_t *pid = REG_GET(old, A1);
                SYSCALL_RETURN(old, terminateProcess(pid));
                if (pid)
                    LDST(old);  // No need to go through scheduler since process is using its time slice fairly
                else
                    start();
                break;
            }
            case VERHOGEN: {
                int *semaddr = REG_GET(old, A1);
                verhogen(semaddr);
                LDST(old);
                break;
            }
            case PASSEREN: {
                int *semaddr = REG_GET(old, A1);
                int blocked = passeren(semaddr, current);
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
                waitIo(command, reg, subdev);
                start();
                break;
            }
            case SPECPASSUP: {
                spu_t type = REG_GET(old, A1);
                state_t *spuOld = REG_GET(old, A2);
                state_t *spuNew = REG_GET(old, A3);
                SYSCALL_RETURN(old, specPassUp(type, spuOld, spuNew));
                LDST(old);
                break;
            }
            case GETPID: {
                memaddr pid = REG_GET(old, A1);
                memaddr ppid = REG_GET(old, A2);
                getPid(current, pid, ppid);
                break;
            }
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

HIDDEN void handleInterrupt(state_t *old, uint8_t line) {
    uint8_t *bitmap = CDEV_BITMAP_ADDR(line);
    int dev = devFromBitmap(*bitmap);
    devreg_t *reg = DEV_REG_ADDR(line, dev);
    int *semKey = getDeviceSemKey(reg);
    pcb_t *p = removeBlocked(semKey);
    addToReadyQueue(p);
    *semKey = 1;  // FIXME: è giusto aggiornare manualmente il valore del semaforo?
    uint32_t status;
    if (line != IL_TERMINAL) {
        dtpreg_t *reg = reg;
        status = reg->status;
        reg->command = CMD_ACK;
    } else {
        termreg_t *reg = reg;
        if (TX_STATUS(reg) == ST_READY)  // Ricezione
            status = reg->recv_status;
        else if (RX_STATUS(reg) == ST_READY)  // Trasmissione
            status = reg->transm_status;
        reg->recv_command = CMD_ACK;
        reg->transm_command = CMD_ACK;
    }
    SYSCALL_RETURN(&p->p_s, status);
    next(old);
}

// TODO: remember to set enter_kernel
void interruptHandler(void) {
    state_t *old = (state_t *)INT_OLDAREA;
#ifdef TARGET_UARM
    PC_SET(old, PC_GET(old) - WORD_SIZE);
#endif
    uint32_t cause = CAUSE_GET(old);

    if (INT_IS_PENDING(cause, IL_TIMER))
        next(old);
    if (INT_IS_PENDING(cause, IL_DISK))
        handleInterrupt(old, IL_DISK);
    if (INT_IS_PENDING(cause, IL_TAPE))
        handleInterrupt(old, IL_TAPE);
    if (INT_IS_PENDING(cause, IL_ETHERNET))
        handleInterrupt(old, IL_ETHERNET);
    if (INT_IS_PENDING(cause, IL_PRINTER))
        handleInterrupt(old, IL_ETHERNET);
    if (INT_IS_PENDING(cause, IL_TERMINAL))
        handleInterrupt(old, IL_TERMINAL);
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
