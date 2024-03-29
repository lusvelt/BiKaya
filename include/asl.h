/*! \file */
#ifndef _ASL_H_
#define _ASL_H_

#include "types.h"

/*!
 * \brief Get the semd associated with the given key
 * 
 * It searches into ASL for the semaphore descriptor
 * associated with the given key. It returns NULL if 
 * the semaphore does not exist on the ASL.
 * \param key key of the semaphore
 * \return semaphore descriptor or NULL
 */
semd_t *asl_semd(int *key);

/*!
 * \brief Initiliazes the Active Semaphore List
 * 
 * It initializes the ASL with all the elements
 * from the semaphore table.
 */
void asl_init();

/*!
 * \brief Inserts a process into the queue of 
 * the given semaphore.
 * 
 * It inserts the given process to the queue of the
 * given semaphore. If the semaphore does not belong
 * to the ASL allocates it from the semdFree list.
 * If semdFree is empty return TRUE and, obviously,
 * does not insert the process. Otherwise returns
 * FALSE and successfully inserts the process.
 * \param key semaphore's key
 * \param p inserted process
 * \returns FALSE if success, TRUE otherwise
 */
bool asl_insert_blocked(int *key, pcb_t *p);

/*! 
 * \brief Removes the first PCB from the queue of the given 
 * semaphore.
 * 
 * This function removes the first PCB from its semaphore's
 *  process queue.
 * If the semaphore does not belong to the ASL the function 
 *  returns NULL.
 * If the process queue becomes empty the function removes the
 *  semaphore from the ASL and adds it to semdFree list.
 * \param key pointer to the semaphore descriptor.
 * \return Removed PCB.
 */
pcb_t *asl_remove_blocked(int *key);

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
pcb_t *asl_find_and_remove_blocked(pcb_t *p);

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
pcb_t *asl_blocked_head(int *key);

/*! 
 * \brief Removes the tree rooted in a PCB.
 * 
 * This function removes the passed PCB and all its children
 *  from the queue of the semaphore in which it is blocked.
 * \param p pointer to the PCB.
 */
void asl_find_and_remove_blocked_child(pcb_t *p);

#endif