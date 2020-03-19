#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "pcb.h"

void start(void);
void addToReadyQueue(pcb_t *p);

#endif