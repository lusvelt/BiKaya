#include "scheduler.h"

#include "asl.h"
#include "listx.h"
#include "system.h"
#include "terminal.h"

HIDDEN LIST_HEAD(readyQueue);
HIDDEN pcb_t *idle = NULL;

HIDDEN pcb_t *getReadyHead() {
    if (list_empty(&readyQueue))
        return NULL;

    return container_of(list_next(&readyQueue), pcb_t, p_next);
}

// maybe should only be included when noone else is in queue, otherwise
// it ends up aging... or put if in aging? kinda trashy..
void idleProcess() {
    while (1) {
        WAIT();
    }
}

// TODO: change name. this starts the head AND RESETS TIME SLICE!
void start(void) {
    // PRECONDITION: at this point, the ready queue head contains the next
    // process that will run
    pcb_t *proc = getReadyHead();

    if (proc == NULL) {
        idle = allocPcb();
        STST(&idle->p_s);
        idle->original_priority = idle->priority = 0;
        STATUS_SET(&idle->p_s, STATUS_ALL_INT_ENABLE(STATUS_GET(&idle->p_s)));
        SET_VM_OFF(&idle->p_s);
        PC_SET(&idle->p_s, idleProcess);
        proc = idle;
        addToReadyQueue(idle);
    } else if (proc != idle) {
        if (outProcQ(&readyQueue, idle))
            freePcb(idle);
    }

    SET_TIMER(TIME_SLICE);
    LDST(&proc->p_s);
}
/*
// TODO: change name. next of what? THIS ONE DOES AGING! and picks who goes
void next(pcb_t *current) {
    // (corrected) INVARIANT: if a process is running, then ready queue head
    // CONTAINED it, before running any handlers! when next is called, we don't know
    // what the head represents. it may be a process awoken by verhogen, that thanks
    // to high priority takes the first spot. This is why the whole pcb_t is needed.
    // Hence, we remove it and we update other processes' priorities (aging)
    // Then, we re-enqueue it in the ready queue

    //remove current, use old area to determine what to do
    
    pcb_t *current = removeProcQ(&readyQueue);
    current->p_s = *currentState;
    // We reset current process priority to avoid inflated priority
    current->priority = current->original_priority;
    
    pcb_t *it;
    list_for_each_entry(it, &readyQueue, p_next)
        it->priority++;

    // following line not needed anymore
    //insertProcQ(&readyQueue, current);
    start();
}
*/

void aging() {
    pcb_t *it;
    list_for_each_entry(it, &readyQueue, p_next)
        it->priority++;
}

void addToReadyQueue(pcb_t *p) {
    insertProcQ(&readyQueue, p);
}

void removeHeadFromReadyQueue() {
    removeProcQ(&readyQueue);
}

void killProgeny(pcb_t *pid) {
    pcb_t *it;
    list_for_each_entry(it, &pid->p_child, p_sib) {
        killProgeny(it);
    }
    outBlocked(pid);  //remove from semaphores (no need for V()
                      //because value changes after process resumes)

    outProcQ(&readyQueue, pid);  //remove from readyQueue
}

pcb_t *getCurrent() {
    return getReadyHead();
}
