#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "types.h"

/* \brief Initializes scheduler.
 *
 * Initializing scheduler entails:
 * - allocating and preparing idle process state
 * - allocating and preparing first process state
 * - setting program counter to code passed as arg
 * - inserting first process in queue
 */
void scheduler_init(pcb_code_t);

void scheduler_enqueue_process(pcb_t *);
void scheduler_kill_process(pid_t);
void scheduler_block_current(int *, state_t *);

/* \brief After ints and sysc handled, resumes execution
 *
 * Multiple cases possible, refer to implementation for details
 */
void scheduler_resume(bool, state_t *);

/* \brief Runs next process from queue
 * 
 * A new process (compared to the last one) runs, so its 
 * time slice is reset. Time spent in kernel is set, and 
 * process runs.
 */
void scheduler_run();

/* \brief Assigns time spent to current_proc as kernel or user
 *
 * Depending on boolean, switch_tick - getTODLO() is assigned
 * to either user or kernel time of current_proc
 */
void scheduler_account_time(bool);

/* \brief Launches exception handler if one was registered
 *
 * If a handler was registered, the handler is launched. 
 * Otherwise, the process and its progeny are terminated
 * Refer to implementation for considerations on location
 */
void scheduler_handle_exception(int, state_t *);

#endif