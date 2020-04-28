#include "scheduler.h"

#include "asl.h"
#include "listx.h"
#include "system.h"

HIDDEN LIST_HEAD(readyQueue);

HIDDEN pcb_t *getReadyHead() {
    if (list_empty(&readyQueue))
        return NULL;

    return container_of(list_next(&readyQueue), pcb_t, p_next);
}

void start(void) {
    // PRECONDITION: at this point, the ready queue head contains the next
    // process that will run
    pcb_t *proc = getReadyHead();
    if (proc == NULL) {
        // For the purpose of phase 1.5, when all the processes terminate the system is halted
        HALT("No more processes to execute.");
    }

    SET_TIMER(TIME_SLICE);
    LDST(&proc->p_s);
}

void next(state_t *currentState) {
    // INVARIANT: when a process is running, ready queue head contains it
    // Hence, we remove it and we update other processes' priorities (aging)
    // Then, we re-enqueue it in the ready queue
    pcb_t *current = removeProcQ(&readyQueue);
    current->p_s = *currentState;
    // We reset current process priority to avoid inflated priority
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

void killProgeny(pcb_t *pid) {
    pcb_t *it;
    list_for_each_entry(it, &p->p_child, p_sib) {
        killProgeny(it);
    }
    outBlocked(pid);  //remove from semaphores (no need for V()
                      //because value changes after process resumes)

    outProcQ(&readyQueue, pid);  //remove from readyQueue
}

pcb_t *getCurrent() {
    return getReadyHead();
}
