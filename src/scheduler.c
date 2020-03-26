#include "scheduler.h"

#include "listx.h"
#include "system.h"
#include "terminal.h"

HIDDEN LIST_HEAD(readyQueue);

HIDDEN pcb_t *getReadyHead() {
    if (list_empty(&readyQueue))
        return NULL;
    return container_of(list_next(&readyQueue), pcb_t, p_next);
}

void start(void) {
    pcb_t *proc = getReadyHead();
    if (proc == NULL) {
        println("No more processes to execute");
        HALT();
    }
    LDST(&proc->p_s);
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

err_t killCurrent() {
    pcb_t *current = getReadyHead();
    if (current == NULL)
        return ERR_READY_QUEUE_EMPTY;
    outChildrenQ(&readyQueue, current);  // Removes current running process and all of its children from ready queue
    return OK;
}