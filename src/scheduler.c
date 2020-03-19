#include "scheduler.h"

#include "const.h"
#include "listx.h"

HIDDEN LIST_HEAD(readyQueue);

void start(void) {
}

void addToReadyQueue(pcb_t *p) {
    insertProcQ(&readyQueue, p);
}