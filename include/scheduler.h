#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "pcb.h"

void start(void);
void next(state_t *);
void addToReadyQueue(pcb_t *p);
err_t killCurrent();

#endif