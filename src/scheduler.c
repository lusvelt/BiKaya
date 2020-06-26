#include "scheduler.h"

#include "asl.h"
#include "const.h"
#include "memory.h"
#include "pcb.h"
#include "terminal.h"

LIST_HEAD(ready_queue);
pcb_t *current_proc = NULL;
uint32_t switch_tick;

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

    init_proc->original_priority = init_proc->priority = DEFAULT_PRIORITY;
    pcb_insert_in_queue(&ready_queue, init_proc);
}

HIDDEN void aging() {
    pcb_t *it;
    list_for_each_entry(it, &ready_queue, p_next) {
        it->priority++;
    }
}

void scheduler_account_time(bool kernel) {
    if (current_proc) {
        if (kernel)
            current_proc->kernel_tm += getTODLO() - switch_tick;
        else
            current_proc->user_tm += getTODLO() - switch_tick;
    }

    switch_tick = getTODLO();
}

void scheduler_resume(bool time_slice_ended, state_t *old_state) {
    if (current_proc) {
        scheduler_account_time(TRUE);

        if (!time_slice_ended)
            LDST(old_state);

        if (!pcb_is_queue_empty(&ready_queue))
            aging();

        memcpy(&current_proc->p_s, old_state, sizeof(state_t));
        // We reset current process priority to avoid inflated priority
        current_proc->priority = current_proc->original_priority;
        pcb_insert_in_queue(&ready_queue, current_proc);
    }

    if (pcb_is_queue_empty(&ready_queue))
        LDST(&idle_proc_state);

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

void scheduler_enqueue_process(pcb_t *proc, bool is_child) {
    proc->priority = proc->original_priority;
    pcb_insert_in_queue(&ready_queue, proc);

    if (is_child)
        pcb_insert_child(current_proc, proc);
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