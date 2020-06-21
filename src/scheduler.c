#include "scheduler.h"

HIDDEN LIST_HEAD(readyQueue);
HIDDEN pcb_t *current_proc = NULL;

HIDDEN void idle_process_code(void) {
    while (1)
        WAIT();
}

void scheduler_init(pcb_code_t code) {
    pcb_t *idle_proc = pcb_alloc();
    STST(&idle_proc->p_s);
    PC(idle_proc->p_s) = idle_process_code;
    STATUS_REG(&idle_proc->p_s) |= KERNEL_MODE | STATUS_ALL_INT_ENABLE(idle_proc->p_s);
    VM(idle_proc->p_s) &= VM_OFF;

    idle_proc->original_priority = idle_proc->priority = IDLE_PRIORITY;
    pcb_insert_in_queue(&readyQueue, idle_proc);

    pcb_t *init_proc = pcb_alloc();
    STST(&init_proc->p_s);
    SP(init_proc->p_s) = RAM_TOP - FRAME_SIZE;
    PC(init_proc->p_s) = code;
    STATUS_REG(init_proc->p_s) |= KERNEL_MODE | STATUS_ALL_INT_ENABLE(init_proc->p_s);
    VM(init_proc->p_s) &= VM_OFF;

    p->original_priority = p->priority = DEFAULT_PRIORITY;
    pcb_insert_in_queue(&readyQueue, init_proc);

    scheduler_resume();
}

void scheduler_resume() {
}