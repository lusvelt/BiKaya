#include "scheduler.h"

#include "asl.h"
#include "const.h"
#include "memory.h"
#include "pcb.h"
#include "terminal.h"

pcb_t *current_proc = NULL;

HIDDEN LIST_HEAD(ready_queue);
HIDDEN uint32_t switch_tick;
HIDDEN state_t idle_proc_state;

HIDDEN void idle_process_code(void) {
    WAIT();
}

void scheduler_init(pcb_code_t code) {
    STST(&idle_proc_state);
    PC(idle_proc_state) = (uint32_t)idle_process_code;
    STATUS(idle_proc_state) = KERNEL_MODE(idle_proc_state);
    STATUS(idle_proc_state) = ALL_INT_ENABLE(idle_proc_state);
    VM(idle_proc_state) &= VM_OFF;

    pcb_t *init_proc = pcb_alloc();
    STST(&init_proc->p_s);
    SP(init_proc->p_s) = RAM_TOP - FRAME_SIZE;
    PC(init_proc->p_s) = (uint32_t)code;
    STATUS(init_proc->p_s) = KERNEL_MODE(init_proc->p_s);
    STATUS(init_proc->p_s) = ALL_INT_ENABLE(init_proc->p_s);
    VM(init_proc->p_s) &= VM_OFF;

    init_proc->original_priority = DEFAULT_PRIORITY;
    scheduler_enqueue_process(init_proc);
}

HIDDEN void aging() {
    pcb_t *it;
    list_for_each_entry(it, &ready_queue, p_next) {
        it->priority++;
    }
}

void scheduler_account_time(bool kernel) {
    // if interrupt was raised while idle was runnning, no point
    // assigning time
    if (current_proc) {
        // depending on param, time spent since last switch is
        // accredited to either kernel or user
        if (kernel)
            current_proc->kernel_tm += getTODLO() - switch_tick;
        else
            current_proc->user_tm += getTODLO() - switch_tick;
    }
    // then switch is reset
    switch_tick = getTODLO();
}

void scheduler_resume(bool time_slice_ended, state_t *old_state) {
    if (current_proc) {
        //account for time spent in kernel
        scheduler_account_time(TRUE);

        // if the process still has time left, it should keep going
        if (!time_slice_ended)
            LDST(old_state);

        // otherwise, if processes are in queue, they should age to avoid
        // starvation by process with highest priority
        if (!pcb_is_queue_empty(&ready_queue))
            aging();

        // process state updated
        memcpy(&current_proc->p_s, old_state, sizeof(state_t));

        scheduler_enqueue_process(current_proc);
    }

    // if no current process and no processes waiting, execute idle
    if (pcb_is_queue_empty(&ready_queue))
        LDST(&idle_proc_state);

    // if no current process but the queue is not empty, a new process
    // should be selected
    scheduler_run();
}

void scheduler_run() {
    // retrieve next process to execute
    current_proc = pcb_remove_from_queue(&ready_queue);

    // If the process is being activated for the first time, we set its start time
    if (!current_proc->start_tm)
        current_proc->start_tm = getTODLO();

    // remember the tick before exiting kernel
    switch_tick = getTODLO();

    // reset timeslice
    setTIMER(TIME_SLICE);

    // load next process state
    LDST(&current_proc->p_s);
}

// add to queue after resetting priority
void scheduler_enqueue_process(pcb_t *proc) {
    // We reset process priority to avoid inflated priority,
    // which would result in starvation
    proc->priority = proc->original_priority;
    // then we enqueue it again so that when a new process is selected
    // it will be the one with highest priority
    pcb_insert_in_queue(&ready_queue, proc);
}

void scheduler_block_current(int *semaddr, state_t *proc_state) {
    memcpy(&current_proc->p_s, proc_state, sizeof(state_t));
    scheduler_account_time(TRUE);
    asl_insert_blocked(semaddr, current_proc);
    current_proc = NULL;
}

HIDDEN void kill_tree(pid_t pid) {
    pid_t it;
    list_for_each_entry(it, &pid->p_child, p_sib) {
        kill_tree(it);
    }

    pcb_find_and_remove_child(pid);

    // remove from semaphores (no need for V()
    // because value changes after process resumes)
    asl_find_and_remove_blocked(pid);

    // remove from readyQueue
    pcb_find_and_remove(&ready_queue, pid);
    pcb_free(pid);
}

void scheduler_kill_process(pid_t pid) {
    if (pid == current_proc)
        current_proc = NULL;

    kill_tree(pid);
}

// although not exactly scheduler's job, it allows us to keep
// current_process local to scheduler and syscall. in a more
// complex kernel, we'd move it to a "system" file
void scheduler_handle_exception(int type, state_t *old_state) {
    scheduler_account_time(FALSE);

    if (current_proc->exc_new_areas[type]) {
        memcpy(current_proc->exc_old_areas[type], old_state, sizeof(state_t));
        scheduler_account_time(TRUE);
        LDST(current_proc->exc_new_areas[type]);
    } else {
        scheduler_kill_process(current_proc);
        scheduler_resume(FALSE, NULL);
    }
}