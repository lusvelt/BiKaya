#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include "types.h"

/*! 
 * \brief Main interrupt handler.
 * 
 * All interrupts are received by interrupts_handler. 
 * Timer interrupt is handled autonomously, while device
 * interrupt handling is delegated to static 
 * handle_interrupt
 */
void interrupts_handler(void);

/*!
 * \brief Returns semaphore for given device
 * 
 * Given a device register (and optionally a 
 * term boolean to account for transmission/reception
 * for terminals), this returns the pointer to
 * the semaphore value assigned to the device
 */
int *interrupts_get_dev_key(devreg_t *, bool);

#endif