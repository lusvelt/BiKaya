#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "types.h"

void scheduler_init(pcb_code_t);
void scheduler_run();
void scheduler_resume(bool time_slice_ended, state_t *old_state);

#endif