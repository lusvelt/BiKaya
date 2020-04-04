#include "scheduler.h"

#include "listx.h"
#include "system.h"
#include "utils.h"

HIDDEN LIST_HEAD(readyQueue);

HIDDEN pcb_t *getReadyHead() {
    if (list_empty(&readyQueue))
        return NULL;

    return container_of(list_next(&readyQueue), pcb_t, p_next);
}

void start(void) {
    pcb_t *proc = getReadyHead();
    if (proc == NULL) {
        EXIT("No more processes to execute.");
    }

    SET_TIMER(TIME_SLICE);
    LDST(&proc->p_s);
}

void next(state_t *currentState) {
    pcb_t *current = removeProcQ(&readyQueue);
    current->p_s = *currentState;
    current->priority = current->original_priority;

    pcb_t *it;
    list_for_each_entry(it, &readyQueue, p_next)
        it->priority++;

    insertProcQ(&readyQueue, current);
    start();
}

void addToReadyQueue(pcb_t *p) {
    insertProcQ(&readyQueue, p);
}

err_t killCurrent() {
    pcb_t *current = getReadyHead();
    if (current == NULL)
        return ERR_READY_QUEUE_EMPTY;

    // Removes current running process and all of its children from ready queue
    outChildrenQ(&readyQueue, current);
    return OK;
}