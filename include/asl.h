#ifndef ASL_H
#define ASL_H

#include <types_bikaya.h>

/*!
 * \brief Get the semd associated with the given key
 * 
 * It searches into ASL for the semaphore descriptor
 * associated with the given key. It returns NULL if 
 * the semaphore does not exist on the ASL.
 * \param key key of the semaphore
 * \return semaphore descriptor or NULL
 */
semd_t *getSemd(int *key);

/*!
 * \brief Initiliazes the Active Semaphore List
 * 
 * It initializes the ASL with all the elements
 * from the semaphore table.
 */
void initASL();

/*!
 * \brief Inserts a process the into queue of 
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
bool insertBlocked(int *key, pcb_t *p);
pcb_t *removeBlocked(int *key);
pcb_t *outBlocked(pcb_t *p);
pcb_t *headBlocked(int *key);
void outChildBlocked(pcb_t *p);

#endif