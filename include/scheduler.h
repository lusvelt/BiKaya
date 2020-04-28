#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "pcb.h"

void start(void);
void next(state_t *);
void addToReadyQueue(pcb_t *p);
void killProgeny(pcb_t *pid);
void getCurrent();

#endif