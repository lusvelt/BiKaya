/*! \file */
#ifndef _PCB_H_
#define _PCB_H_

#include "types.h"

/*! 
 * \brief Initializes pcbFree list.
 * 
 * This function initializes pcbFree list in order to contain
 *  pcbTable elements.
 */
void initPcbs(void);

/*! 
 * \brief Inserts a PCB to the list of free ones.
 * 
 * This function inserts a previously allococated PCB to the list
 *  of unallocated ones.
 * \param p pointer to the PCB to deallocate
 */
void freePcb(pcb_t *p);

/*! 
 * \brief Removes a PCB from the free list.
 * 
 * This function removes a PCB from the free list and returns it.
 * If the list is empty the function returns NULL.
 * \return removed PCB.
 */
pcb_t *allocPcb(void);

/*! 
 * \brief Initializes a queue.
 * 
 * This function initializes the queue head initializing its
 *  dummy element.
 * \param head pointer to the dummy element.
 */
void mkEmptyProcQ(struct list_head *head);

/*! 
 * \brief Controls if empty.
 * 
 * This function controls if the passed queue is empty.
 * \param head pointer to the queue.
 * \return TRUE if the queue is empty, FALSE otherwise.
 */
bool emptyProcQ(struct list_head *head);

/*! 
 * \brief Inserts a PCB into a queue.
 * 
 * This function inserts the passed PCB into the queue
 *  respecting its priority.
 * \param head pointer to the queue.
 * \param p pointer to the PCB to insert.
 */
void insertProcQ(struct list_head *head, pcb_t *p);

/*! 
 * \brief Returns head element.
 * 
 * This function returns the head element of the queue
 *  without removing it.
 * It returns NULL if the queue is empty.
 * \param head pointer to the queue.
 * \return head element.
 */
pcb_t *headProcQ(struct list_head *head);

/*! 
 * \brief Removes head element.
 * 
 * This function removes the head element of the queue.
 * It returns NULL if the queue is empty.
 * \param head pointer to the queue.
 * \return head element.
 */
pcb_t *removeProcQ(struct list_head *head);

/*! 
 * \brief Removes a PCB.
 * 
 * This function removes the passed PCB from the queue.
 * It returns NULL if the queue is empty or the PCB is
 *  not present.
 * \param head pointer to the queue.
 * \param p pointer to the PCB to remove.
 * \return removed PCB.
 */
pcb_t *outProcQ(struct list_head *head, pcb_t *p);

/*! 
 * \brief Controls if a PCB has children.
 * 
 * This function controls if the passed PCB has children.
 * \param this pointer to the PCB.
 * \return TRUE if the PCB has no children, FALSE otherwise.
 */
bool emptyChild(pcb_t *this);

/*! 
 * \brief Inserts a child.
 * 
 * This function inserts the passed PCB as a child of
 *  another PCB.
 * \param parent pointer to the PCB.
 * \param p pointer to the PCB to add as a child.
 */
void insertChild(pcb_t *parent, pcb_t *p);

/*! 
 * \brief Removes a child.
 * 
 * This function removes the first child of the passed PCB.
 * If the passed PCB has no child the function returns NULL.
 * \param p pointer to the PCB.
 * \return Removed child.
 */
pcb_t *removeChild(pcb_t *p);

/*! 
 * \brief Removes a child.
 * 
 * This function removes the passed PCB from its parent's list.
 * If the passed PCB has no father the function returns NULL.
 * \param p pointer to the child to remove.
 * \return Removed child.
 */
pcb_t *outChild(pcb_t *p);

#endif
