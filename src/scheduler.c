#include "scheduler.h"

#include "listx.h"
#include "system.h"

HIDDEN LIST_HEAD(readyQueue);

void start(void) {
    pcb_t *ready = container_of(list_next(&readyQueue), pcb_t, p_next);
    LDST(&ready->p_s);
}

// err_t createProcess(pcb_handler_t handler, uint8_t priority) {
//     static uint8_t processes = 0;

//     pcb_t *pcb = allocPcb();
//     if (pcb == NULL) return ERR_NO_PROC;  // No more space for processes

//     processes++;

//     // 1. Initialize process state
//     state_t *state = &pcb->p_s;
//     SP_SET(state, RAM_TOP - FRAME_SIZE * processes);
//     INT_DISABLE(state);
//     TIMER_ENABLE(state);
//     KERNEL_MODE(state);
//     VM_ENABLE(state);
//     PC_SET(state, handler);

//     // 2. Set its priorities
//     pcb->original_priority = pcb->priority = priority;

//     // 3. Add to the ready queue
//     insertProcQ(&readyQueue, pcb);

//     // 4. Return a success code
//     return OK;
// }

void addToReadyQueue(pcb_t *p) {
    insertProcQ(&readyQueue, p);
}