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
HIDDEN state_t *old_state;

HIDDEN int create_process(state_t *state, int priority, pid_t *cpid) {
    pcb_t *new_proc = pcb_alloc();
    if (new_proc == NULL)
        return SYSCALL_FAILURE;

    memcpy(&new_proc->p_s, state, sizeof(state_t));

    new_proc->original_priority = priority;
    scheduler_enqueue_process(new_proc);  // This also set priority to original_priority
    pcb_insert_child(current_proc, new_proc);

    if (cpid) *cpid = new_proc;

    return SYSCALL_SUCCESS;
}

int terminate_process(pid_t pid) {
    if (pid == NULL)
        pid = current_proc;

    // Check if pid belongs to some queue (i.e. it does not belong
    // to the pcb free list)
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
        // This also reset process priority to original_priority
        scheduler_enqueue_process(blocked);
    } else
        *semaddr += 1;
}

HIDDEN void passeren(int *semaddr) {
    if (*semaddr)
        *semaddr -= 1;
    else
        // Block current process on semaphore semaddr
        scheduler_block_current(semaddr, old_state);
}

HIDDEN void wait_io(uint32_t command, devreg_t *device, bool subdev) {
    if (IS_TERMINAL(device)) {
        termreg_t *terminal = (termreg_t *)device;

        if (subdev)
            terminal->recv_command = command;
        else
            terminal->transm_command = command;
    } else {
        dtpreg_t *dtp = (dtpreg_t *)device;
        dtp->command = command;
    }

    int *dev_sem_key = interrupts_get_dev_key(device, subdev);

    // since calling passeren would always have the same effect, we
    // skip the if and block the process right away
    scheduler_block_current(dev_sem_key, old_state);
}

HIDDEN int spec_pass_up(int exc_type, state_t *old_area, state_t *new_area) {
    if (current_proc->exc_new_areas[exc_type]) {
        scheduler_kill_process(current_proc);
        return SYSCALL_FAILURE;
    }

    current_proc->exc_new_areas[exc_type] = new_area;
    current_proc->exc_old_areas[exc_type] = old_area;

    return SYSCALL_SUCCESS;
}

void syscall_handler(void) {
    // As usual, start by accounting user time
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

                // account kernel time, to return the most accurate
                // value as possible
                scheduler_account_time(TRUE);
                *kernel = current_proc->kernel_tm;
                break;
            }
            case CREATEPROCESS:
                SYSRETURN(*old_state) =
                    create_process(
                        (state_t *)SYSARG1(*old_state),
                        (int)SYSARG2(*old_state),
                        (pid_t *)SYSARG3(*old_state));
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
                wait_io(
                    (uint32_t)SYSARG1(*old_state),
                    (devreg_t *)SYSARG2(*old_state),
                    (bool)SYSARG3(*old_state));
                break;
            case SPECPASSUP:
                SYSRETURN(*old_state) =
                    spec_pass_up(
                        SYSARG1(*old_state),
                        (state_t *)SYSARG2(*old_state),
                        (state_t *)SYSARG3(*old_state));
                break;
            case GETPID: {
                pid_t *pid = (pid_t *)SYSARG1(*old_state);
                if (pid) *pid = current_proc;

                pid_t *ppid = (pid_t *)SYSARG2(*old_state);
                if (ppid) *ppid = current_proc->p_parent;
                break;
            }
            default:
                // Simply delegates to the scheduler passing the right exception type
                scheduler_handle_exception(
                    SPECPASSUP_SYSBK_TYPE,
                    (state_t *)SYSBK_OLDAREA);
                break;
        }
    }

    // Here, we have 2 situation possible:
    // 1. current_proc has been blocked somewhere or terminated, so the scheduler
    //    will have to select another process to execute
    // 2. current_proc is alive and, theoretically, it still has time to execute
    //    (i.e. its time slice isn't finished)
    scheduler_resume(FALSE, old_state);
}