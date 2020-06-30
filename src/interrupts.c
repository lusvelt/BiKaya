#include "interrupts.h"

#include "asl.h"
#include "const.h"
#include "memory.h"
#include "pcb.h"
#include "scheduler.h"
#include "terminal.h"
#include "types.h"

// since the two terminal functions (reception/transmission)
// work independently and concurrently, we added
// a column to account for it (i.e index 4 is for transmission
// requests, 5 for reception)
HIDDEN int devices_semkeys[N_EXT_IL + 1][N_DEV_PER_IL];

// Utility function to remove the first process blocked on the given
// semaphore and reinsert it into the ready queue.
HIDDEN inline void unblock_process(int *semkey, uint32_t status) {
    pcb_t *unblocked = asl_remove_blocked(semkey);
    scheduler_enqueue_process(unblocked);
    SYSRETURN(unblocked->p_s) = status;
}

HIDDEN void handle_dtp(dtpreg_t *device) {
    int *dev_sem_key = interrupts_get_dev_key((devreg_t *)device, FALSE);
    unblock_process(dev_sem_key, device->status);
    device->command = CMD_ACK;
}

HIDDEN void handle_term(termreg_t *device) {
    if (TX_STATUS(device) == ST_TRANSMITTED) {
        int *dev_sem_key = interrupts_get_dev_key((devreg_t *)device, FALSE);
        unblock_process(dev_sem_key, device->transm_status);
        device->transm_command = CMD_ACK;
    }
    // cannot be mutually exclusive as both are concurrent and independent
    if (RX_STATUS(device) == ST_RECEIVED) {
        int *dev_sem_key = interrupts_get_dev_key((devreg_t *)device, TRUE);
        unblock_process(dev_sem_key, device->recv_status);
        device->recv_command = CMD_ACK;
    }
}

HIDDEN void handle_interrupt(uint8_t line) {
    uint8_t bitmap = *((uint8_t *)CDEV_BITMAP_ADDR(line));

    // we shift each time to check if any device has interrupts raised,
    // then we shift right to shorten the number (equal to divide by 2)
    for (int i = 0; i < 8; i++) {
        if (bitmap % 2) {
            devreg_t *device = (devreg_t *)DEV_REG_ADDR(line, i);

            // Terminal and DTP devices must be handled separately
            if (IS_TERMINAL(device))
                handle_term((termreg_t *)device);
            else
                handle_dtp((dtpreg_t *)device);
        }

        bitmap >>= 1;
    }
}

void interrupts_handler(void) {
    // as other kernel handlers, time spent in interrupts_handler
    // is accounted for as kernel time
    scheduler_account_time(FALSE);
    state_t *old_state = (state_t *)INT_OLDAREA;

    uint32_t cause = CAUSE(*old_state);
    bool time_slice_ended = FALSE;

#ifdef TARGET_UARM
    PC(*old_state) -= WORD_SIZE;
#endif

    if (INT_IS_PENDING(cause, IL_TIMER)) {
        setTIMER(TIMER_ACK);      // ack interrupt, timeslice will be set in sched
        time_slice_ended = TRUE;  // we have to remember that time slice ended
    }

    if (INT_IS_PENDING(cause, IL_DISK))
        handle_interrupt(IL_DISK);

    if (INT_IS_PENDING(cause, IL_TAPE))
        handle_interrupt(IL_TAPE);

    if (INT_IS_PENDING(cause, IL_ETHERNET))
        handle_interrupt(IL_ETHERNET);

    if (INT_IS_PENDING(cause, IL_PRINTER))
        handle_interrupt(IL_ETHERNET);

    if (INT_IS_PENDING(cause, IL_TERMINAL))
        handle_interrupt(IL_TERMINAL);

    // After handling all interrupts we resume scheduler.
    // It needs to know if time slice ended (i.e. there was a timer interrupt),
    // and receives the old_state to be assigned to the current_proc (before
    // the context switch).
    scheduler_resume(time_slice_ended, old_state);
}

// Device semaphores belong to a matrix of 6 rows (one for each device type
// plus one accounting for terminal reception) and 8 columns (one for each
// device, 8 for each line). This function, thanks to some magic math, allows
// us to retrieve the right semaphore associated with the device from its
// register address.
// [
//   [0, 0, 0, 0, 0, 0, 0, 0], (3  disks            )
//   [0, 0, 0, 0, 0, 0, 0, 0], (4  tapes            )
//   [0, 0, 0, 0, 0, 0, 0, 0], (5  net adapters     )
//   [0, 0, 0, 0, 0, 0, 0, 0], (6  printers         )
//   [0, 0, 0, 0, 0, 0, 0, 0], (7  terminals transm )
//   [0, 0, 0, 0, 0, 0, 0, 0], (7b terminals rcpt   )
// ]
int *interrupts_get_dev_key(devreg_t *reg, bool term_rcpt) {
    // amount of devices before reg
    int dev_abs_index = ((uint32_t)reg - DEV_REG_START) / DEV_REG_SIZE;
    // position relative to a specific line, [0-7]
    int dev_rel_index = dev_abs_index % N_DEV_PER_IL;
    int dev_line = dev_abs_index / N_DEV_PER_IL;  // [0-4]

    if (DEV_IL_START + dev_line < IL_TERMINAL)
        return &devices_semkeys[dev_line][dev_rel_index];
    else
        return &devices_semkeys[dev_line + term_rcpt][dev_rel_index];
}