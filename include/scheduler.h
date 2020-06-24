#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "types.h"

void scheduler_init(pcb_code_t);
void scheduler_run();
void scheduler_resume(state_t *current_proc_state, bool time_slice_ended);

#endif