#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "types.h"

void scheduler_init(pcb_code_t);
void scheduler_enqueue_process(pcb_t *proc, bool is_child);
void scheduler_kill_process(pid_t pid);
void scheduler_block_current(int *semaddr, state_t *proc_state);
void scheduler_run();
void scheduler_resume(bool time_slice_ended, state_t *old_state);
void scheduler_account_time(bool kernel);
void scheduler_handle_exception(int type, state_t *old_state);

#endif