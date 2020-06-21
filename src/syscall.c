#include "syscall.h"

HIDDEN syscall_ret_t syscall_create_process(state_t *state, int priority, void **cpid) {
    pcb_t *p = pcb_alloc();

    if (p == NULL)
        return SYSCALL_FAILURE;

    p->p_s = *state;

    p->original_priority = priority;
    p->priority = priority;

    pcb_t *current = getCurrent();
    pcb_insert_child(current, p);

    addToReadyQueue(p);

    if (cpid)
        *cpid = p;

    return SYSCALL_SUCCESS;
}

void syscall_handler(void) {
    state_t old_state = *((state_t *)SYSBK_OLDAREA);

    if (CAUSE(old_state) == CAUSE_SYSCALL) {
#ifdef TARGET_UMPS
        PC(old_state) += WORD_SIZE;
#endif

        uint32_t syscall_no = SYSARG0(old_state);

        switch (syscall_no) {
            case GETCPUTIME:
                break;
            case CREATEPROCESS: {
                state_t *state = SYSARG1(old_state);
                int priority = SYSARG2(old_state);
                void **cpid = SYSARG3(old_state);

                SYSRETURN(old_state) = create_process(state, priority, cpid);
                break;
            }
            case TERMINATEPROCESS: {
                pcb_t *pid = SYSARG1(old_state);
            }
            case VERHOGEN: {
                int *semaddr = SYSARG1(old_state);
            }
            case PASSEREN: {
                int *semaddr = SYSARG1(old_state);
            }
            case WAITIO: {
                uint32_t command = SYSARG1(old_state);
                uint32_t *reg = SYSARG2(old_state);
                bool subdev = SYSARG3(old_state);
            }
            case SPECPASSUP: {
                spu_t type = SYSARG1(old_state);
                state_t *spuOld = SYSARG2(old_state);
                state_t *spuNew = SYSARG3(old_state);
            }
            case GETPID: {
                uint32_t *pid = SYSARG1(old_state);
                uint32_t *ppid = SYSARG2(old_state);
            }
            default:
                break;
        }

        scheduler_resume();
    }
}