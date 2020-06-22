#include "syscall.h"

#include "const.h"
// including interrupts just for get_dev_key function
#include "asl.h"
#include "interrupts.h"
#include "macro.h"
#include "memory.h"
#include "pcb.h"
#include "types.h"

#define SYSCALL_SUCCESS (0)
#define SYSCALL_FAILURE (-1)

extern struct list_head ready_queue;
extern pcb_t *current_proc;
extern void scheduler_resume();

HIDDEN int create_process(state_t *state, int priority, pid_t *cpid) {
    pcb_t *new_proc = pcb_alloc();
    if (new_proc == NULL)
        return SYSCALL_FAILURE;

    memcpy(&new_proc->p_s, state, sizeof(state_t));
    new_proc->original_priority = new_proc->priority = priority;

    pcb_insert_child(current_proc, new_proc);
    pcb_insert_in_queue(&ready_queue, new_proc);

    if (cpid) *cpid = new_proc;

    return SYSCALL_SUCCESS;
}

HIDDEN void kill_progeny(pid_t pid) {
    pid_t it;
    list_for_each_entry(it, &pid->p_child, p_sib) {
        kill_progeny(it);
    }

    pcb_find_and_remove_child(pid);

    // remove from semaphores (no need for V()
    // because value changes after process resumes)
    asl_find_and_remove_blocked(pid);

    // remove from readyQueue
    pcb_find_and_remove(&ready_queue, pid);
    pcb_free(pid);
}

HIDDEN int terminate_process(pid_t pid) {
    if (pcb_is_free(pid)) return SYSCALL_FAILURE;

    kill_progeny(pid);
    return SYSCALL_SUCCESS;
}

HIDDEN void verhogen(int *semaddr) {
    semd_t *semd = asl_semd(semaddr);

    // No need to check if queue is empty because `asl_remove_blocked`
    // frees semd as soon as its queue becomes empty
    if (semd) {
        pcb_t *blocked = asl_remove_blocked(semaddr);
        pcb_insert_in_queue(&ready_queue, blocked);
    } else {
        *semaddr += 1;
    }
}

HIDDEN void passeren(int *semaddr) {
    if (*semaddr) {
        *semaddr -= 1;
    } else {
        // Current process doesn't belong to the ready queue,
        // so we can simply add it to the semaphore queue
        asl_insert_blocked(semaddr, current_proc);
        current_proc = NULL;
    }
}

// TODO: consider removing macro or find better way
#define SET_COMMAND(reg, subdev, command) (*((uint32_t *)(reg) + 1 + (2 * (1 - subdev))) = (command))

HIDDEN void wait_io(uint32_t command, devreg_t *dev_reg, bool subdev) {
    SET_COMMAND(dev_reg, subdev, command);

    int *device_semkey = interrupts_get_dev_key(dev_reg);
    if (asl_insert_blocked(device_semkey, current_proc)) {
        EXIT("Too many semaphores allocated.");
    }
}

HIDDEN int spec_pass_up(int exc_type, state_t *old_area, state_t *new_area) {
    if (current_proc->exc_new_areas[exc_type]) {
        terminate_process(current_proc);
        return SYSCALL_FAILURE;
    }

    current_proc->exc_new_areas[exc_type] = new_area;
    current_proc->exc_old_areas[exc_type] = old_area;

    return SYSCALL_SUCCESS;
}

void syscalls_handler(void) {
    state_t old_state = *((state_t *)SYSBK_OLDAREA);

    if (CAUSE(old_state) == CAUSE_SYSCALL) {
#ifdef TARGET_UMPS
        PC(old_state) += WORD_SIZE;
#endif

        syscall_t syscall_no = SYSARG0(old_state);

        switch (syscall_no) {
            case GETCPUTIME:
                break;
            case CREATEPROCESS: {
                state_t *proc_state = (state_t *)SYSARG1(old_state);
                int proc_priority = SYSARG2(old_state);
                pid_t *cpid = (pid_t *)SYSARG3(old_state);

                SYSRETURN(old_state) = create_process(proc_state, proc_priority, cpid);
                break;
            }
            case TERMINATEPROCESS: {
                pid_t pid = (pid_t)SYSARG1(old_state);
                if (pid == NULL) pid = current_proc;

                //TODO: check that pid exists (i.e. if it belongs to some queue)
                SYSRETURN(old_state) = terminate_process(pid);
                break;
            }
            case VERHOGEN:
                verhogen((int *)SYSARG1(old_state));
                break;
            case PASSEREN:
                passeren((int *)SYSARG1(old_state));
                break;
            case WAITIO:
                wait_io((uint32_t)SYSARG1(old_state), (devreg_t *)SYSARG2(old_state), (bool)SYSARG3(old_state));
                break;
            case SPECPASSUP:
                SYSRETURN(old_state) = spec_pass_up(SYSARG1(old_state), (state_t *)SYSARG2(old_state), (state_t *)SYSARG3(old_state));
                break;
            case GETPID: {
                pid_t *pid = (pid_t *)SYSARG1(old_state);
                if (pid) *pid = current_proc;

                pid_t *ppid = (pid_t *)SYSARG2(old_state);
                if (ppid) *ppid = current_proc->p_parent;
            }
            default:
                if (current_proc->exc_new_areas[SPECPASSUP_SYSBK_TYPE]) {
                    memcpy(current_proc->exc_old_areas[SPECPASSUP_SYSBK_TYPE], &old_state, sizeof(state_t));
                    LDST(current_proc->exc_new_areas[SPECPASSUP_SYSBK_TYPE]);
                } else {
                    terminate_process(current_proc);
                }
                break;
        }

        scheduler_resume();
    }
}