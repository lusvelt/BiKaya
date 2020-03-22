#include "scheduler.h"

#include "const.h"
#include "listx.h"
#include "system.h"
#include "term.h"

extern void test1();

HIDDEN LIST_HEAD(readyQueue);

void start(void) {
    pcb_t *pcb = container_of(list_next(&readyQueue), pcb_t, p_next);
    LDST(&pcb->p_s);
}

void addToReadyQueue(pcb_t *p) {
    insertProcQ(&readyQueue, p);
}