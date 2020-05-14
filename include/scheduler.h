#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "const.h"
#include "pcb.h"

void start(void);
void next(state_t *);
void addToReadyQueue(pcb_t *p);
void removeHeadFromReadyQueue();
void killProgeny(pcb_t *pid);
pcb_t *getCurrent();
void aging();

#ifdef DEBUG
void printReadyQueue();
#endif

#endif