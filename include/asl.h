#ifndef ASL_H
#define ASL_H

#include <types_bikaya.h>

/* ASL handling functions */
semd_t *getSemd(int *key);
void initASL();

int insertBlocked(int *key, pcb_t *p);

/*! 
 * \brief Removes a PCB from its queue.
 * 
 * This function removes the first PCB from its semaphore's
 *  process queue.
 * If the semaphore does not belong to the ASL the function 
 *  returns NULL.
 * If the process queue becomes empty the function removes the
 *  semaphore from the ASL and adds it to semdFree list.
 * \param key pointer to the key of a semaphore.
 * \return Removed PCB.
 */
pcb_t *removeBlocked(int *key);

/*! 
 * \brief Removes a PCB from its queue.
 * 
 * This function removes the passed PCB from its semaphore's
 *  process queue.
 * If the semaphore does not belong to the ASL the function 
 *  returns NULL.
 * If the process queue becomes empty the function removes the
 *  semaphore from the ASL and adds it to semdFree list.
 * \param p pointer to the PCB to remove.
 * \return Removed PCB.
 */
pcb_t *outBlocked(pcb_t *p);

/*! 
 * \brief Returns head PCB of a semaphore.
 * 
 * This function returns the first element of the given
 *  semaphore's process queue.
 * If the semaphore is not present in the ASL or his process
 *  queue is empty the function returns NULL.
 *  
 * \param key pointer to the key of a semaphore.
 * \return head PCB.
 */
pcb_t *headBlocked(int *key);

/*! 
 * \brief Removes the tree rooted in a PCB.
 * 
 * This function removes the passed PCB and all its children
 *  from the queue of the semaphore in which it is blocked.
 * \param p pointer to the PCB.
 */
void outChildBlocked(pcb_t *p);

#endif