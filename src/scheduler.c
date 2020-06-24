#include "scheduler.h"

#include "const.h"
#include "memory.h"
#include "pcb.h"
#include "terminal.h"

LIST_HEAD(ready_queue);
pcb_t *current_proc = NULL;

HIDDEN void idle_process_code(void) {
    while (1)
        WAIT();
}

void scheduler_init(pcb_code_t code) {
    pcb_t *idle_proc = pcb_alloc();
    STST(&idle_proc->p_s);
    PC(idle_proc->p_s) = (uint32_t)idle_process_code;
    STATUS(idle_proc->p_s) = KERNEL_MODE(idle_proc->p_s);
    STATUS(idle_proc->p_s) = ALL_INT_ENABLE(idle_proc->p_s);
    VM(idle_proc->p_s) &= VM_OFF;

    idle_proc->original_priority = idle_proc->priority = IDLE_PRIORITY;
    pcb_insert_in_queue(&ready_queue, idle_proc);

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

void scheduler_resume(bool time_slice_ended) {
    if (current_proc) {
        if (!time_slice_ended)
            LDST(&current_proc->p_s);

        if (!pcb_is_queue_empty(&ready_queue))
            aging();

        // We reset current process priority to avoid inflated priority
        current_proc->priority = current_proc->original_priority;
        pcb_insert_in_queue(&ready_queue, current_proc);
    }

    scheduler_run();
}

void scheduler_run() {
    // retrieve next process to execute
    current_proc = pcb_remove_from_queue(&ready_queue);
    debugln("current_proc = %p", current_proc->p_next);

    // reset timeslice
    setTIMER(TIME_SLICE);

    // load next process state
    LDST(&current_proc->p_s);
}
