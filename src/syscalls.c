#include "syscalls.h"

#include "const.h"
// including interrupts just for get_dev_key function
#include "asl.h"
#include "interrupts.h"
#include "macro.h"
#include "memory.h"
#include "pcb.h"
#include "scheduler.h"
#include "terminal.h"
#include "types.h"

#define SYSCALL_SUCCESS (0)
#define SYSCALL_FAILURE (-1)

extern pcb_t *current_proc;
state_t *old_state;

HIDDEN int create_process(state_t *state, int priority, pid_t *cpid) {
    pcb_t *new_proc = pcb_alloc();
    if (new_proc == NULL)
        return SYSCALL_FAILURE;

    memcpy(&new_proc->p_s, state, sizeof(state_t));

    new_proc->original_priority = priority;
    scheduler_enqueue_process(new_proc, TRUE);  // This also set priority to original_priority

    if (cpid) *cpid = new_proc;

    return SYSCALL_SUCCESS;
}

int terminate_process(pid_t pid) {
    if (pcb_is_free(pid))
        return SYSCALL_FAILURE;

    scheduler_kill_process(pid);

    return SYSCALL_SUCCESS;
}

HIDDEN void verhogen(int *semaddr) {
    semd_t *semd = asl_semd(semaddr);

    // No need to check if queue is empty because `asl_remove_blocked`
    // frees semd as soon as its queue becomes empty
    if (semd) {
        pcb_t *blocked = asl_remove_blocked(semaddr);
        scheduler_enqueue_process(blocked, FALSE);
    } else
        *semaddr += 1;
}

HIDDEN void passeren(int *semaddr) {
    if (*semaddr)
        *semaddr -= 1;
    else
        scheduler_block_current(semaddr, old_state);
}

// TODO: consider removing macro or find better way
#define SET_COMMAND(reg, subdev, command) (*((uint32_t *)(reg) + 1 + (2 * (1 - subdev))) = (command))

HIDDEN void wait_io(uint32_t command, devreg_t *dev_reg, bool subdev) {
    SET_COMMAND(dev_reg, subdev, command);

    int *dev_sem_key = interrupt_get_dev_key(dev_reg, subdev);
    scheduler_block_current(dev_sem_key, old_state);
}

HIDDEN int spec_pass_up(int exc_type, state_t *old_area, state_t *new_area) {
    if (current_proc->exc_new_areas[exc_type]) {
        scheduler_kill_process(NULL);
        return SYSCALL_FAILURE;
    }

    current_proc->exc_new_areas[exc_type] = new_area;
    current_proc->exc_old_areas[exc_type] = old_area;

    return SYSCALL_SUCCESS;
}

void syscall_handler(void) {
    scheduler_account_time(FALSE);
    old_state = (state_t *)SYSBK_OLDAREA;

    if (CAUSE_EXC(*old_state) == CAUSE_SYSCALL) {
#ifdef TARGET_UMPS
        PC(*old_state) += WORD_SIZE;
#endif

        int syscall_no = SYSARG0(*old_state);
        switch (syscall_no) {
            case GETCPUTIME: {  // user-kernel-wall
                uint32_t *user = (uint32_t *)SYSARG1(*old_state);
                uint32_t *kernel = (uint32_t *)SYSARG2(*old_state);
                uint32_t *wallclock = (uint32_t *)SYSARG3(*old_state);

                *wallclock = getTODLO() - current_proc->start_tm;
                *user = current_proc->user_tm;

                scheduler_account_time(TRUE);
                *kernel = current_proc->kernel_tm;
                break;
            }
            case CREATEPROCESS:
                SYSRETURN(*old_state) = create_process((state_t *)SYSARG1(*old_state), (int)SYSARG2(*old_state), (pid_t *)SYSARG3(*old_state));
                break;
            case TERMINATEPROCESS: {
                pid_t pid = (pid_t)SYSARG1(*old_state);
                SYSRETURN(*old_state) = terminate_process(pid);
                break;
            }
            case VERHOGEN:
                // If a process with higher priority than current one appears after
                // verhogen it doesn't stop current process.
                verhogen((int *)SYSARG1(*old_state));
                break;
            case PASSEREN:
                passeren((int *)SYSARG1(*old_state));
                break;
            case WAITIO:
                wait_io((uint32_t)SYSARG1(*old_state), (devreg_t *)SYSARG2(*old_state), (bool)SYSARG3(*old_state));
                break;
            case SPECPASSUP:
                SYSRETURN(*old_state) = spec_pass_up(SYSARG1(*old_state), (state_t *)SYSARG2(*old_state), (state_t *)SYSARG3(*old_state));
                break;
            case GETPID: {
                pid_t *pid = (pid_t *)SYSARG1(*old_state);
                if (pid) *pid = current_proc;

                pid_t *ppid = (pid_t *)SYSARG2(*old_state);
                if (ppid) *ppid = current_proc->p_parent;
                break;
            }
            default:
                scheduler_handle_exception(SPECPASSUP_SYSBK_TYPE, (state_t *)SYSBK_OLDAREA);
                break;
        }
    }

    scheduler_resume(FALSE, old_state);
}