#include "interrupts.h"

#include "asl.h"
#include "const.h"
#include "memory.h"
#include "pcb.h"
#include "scheduler.h"  // <-- are these supposed to be in interrupts.h ?
#include "terminal.h"
#include "types.h"

// since the two terminal functions (reception/transmission)
// work independently and concurrently, we added
// a column to account for it (i.e index 4 is for transmission
// requests, 5 for reception)
HIDDEN int devices_semkeys[N_EXT_IL + 1][N_DEV_PER_IL];
extern struct list_head ready_queue;

HIDDEN inline void unblock_process(int *semkey, uint32_t status) {
    pcb_t *unblocked = asl_remove_blocked(semkey);
    pcb_insert_in_queue(&ready_queue, unblocked);
    SYSRETURN(unblocked->p_s) = status;
}

HIDDEN void handle_dtp(dtpreg_t *device) {
    device->command = CMD_ACK;
    int *dev_sem_key = interrupts_get_dev_key((devreg_t *)device, FALSE);
    unblock_process(dev_sem_key, device->status);
}

HIDDEN void handle_term(termreg_t *device) {
    if (TX_STATUS(device) != ST_BUSY) {
        device->transm_command = CMD_ACK;
        int *dev_sem_key = interrupts_get_dev_key((devreg_t *)device, FALSE);
        unblock_process(dev_sem_key, TX_STATUS(device));
    }
    // cannot be mutually exclusive as both are concurrent and independent
    if (RX_STATUS(device) != ST_BUSY) {
        device->recv_command = CMD_ACK;
        int *dev_sem_key = interrupts_get_dev_key((devreg_t *)device, TRUE);
        unblock_process(dev_sem_key, RX_STATUS(device));
    }
}

HIDDEN void handle_interrupt(uint8_t line) {
    uint8_t bitmap = *((uint8_t *)CDEV_BITMAP_ADDR(line));

    // we shift each time to check if any device has interrupts raised,
    // then we shift right to shorten the number (equal to divide by 2)
    for (int i = 0; i < 8; i++) {
        if (bitmap % 2) {
            devreg_t *device = (devreg_t *)DEV_REG_ADDR(line, i);
            if (IS_TERMINAL(device))
                handle_term((termreg_t *)device);
            else
                handle_dtp((dtpreg_t *)device);
        }

        bitmap >>= 1;
    }
}

void interrupts_handler(void) {
    state_t old_state;
    memcpy(&old_state, (state_t *)INT_OLDAREA, sizeof(state_t));

    uint32_t cause = CAUSE(old_state);
    bool time_slice_ended = FALSE;

#ifdef TARGET_UARM
    PC(old_state) -= WORD_SIZE;
#endif

    if (INT_IS_PENDING(cause, IL_TIMER)) {
        setTIMER(TIMER_ACK);  // ack interrupt, timeslice will be set in sched
        time_slice_ended = TRUE;
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

    scheduler_resume(&old_state, time_slice_ended);
}

// given a device register (and optionally a term boolean to account for
// transmission/reception for terminals), this returns the pointer to
// the semaphore value assigned to the device
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
/*
[
[0, 0, 0, 0, 0, 0, 0, 0], (3  disks            )
[0, 0, 0, 0, 0, 0, 0, 0], (4  tapes            )
[0, 0, 0, 0, 0, 0, 0, 0], (5  net adapters     )
[0, 0, 0, 0, 0, 0, 0, 0], (6  printers         )
[0, 0, 0, 0, 0, 0, 0, 0], (7  terminals transm )
[0, 0, 0, 0, 0, 0, 0, 0], (7b terminals rcpt   )
]
*/