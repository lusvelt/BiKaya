#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "pcb.h"

void start(void);
// err_t createProcess(pcb_handler_t handler, uint8_t priority);
void next(state_t *);

void addToReadyQueue(pcb_t *p);

err_t killCurrent();

#endif