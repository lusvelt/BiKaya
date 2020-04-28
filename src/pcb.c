#include "pcb.h"

#include "const.h"
#include "memory.h"
#include "terminal.h"

// only relevant to this file, so HIDDEN (static)
HIDDEN pcb_t pcbTable[MAXPROC];
HIDDEN LIST_HEAD(pcbFree);  // dummy

/* PCB free list handling functions */
void initPcbs(void) {
    int i;

    for (i = 0; i < MAXPROC; i++)
        list_add(&(pcbTable[i].p_next), &pcbFree);
}

void freePcb(pcb_t *p) {
    list_add(&(p->p_next), &pcbFree);
}

pcb_t *allocPcb(void) {
    struct list_head *next = list_next(&pcbFree);

    if (next == NULL)
        return NULL;

    list_del(next);
    pcb_t *pcb = container_of(next, pcb_t, p_next);

    //initialize fields
    INIT_LIST_HEAD(&(pcb->p_child));
    INIT_LIST_HEAD(&(pcb->p_next));
    INIT_LIST_HEAD(&(pcb->p_sib));
    pcb->p_parent = NULL;
    pcb->p_semkey = NULL;
    pcb->priority = 0;
    // this {0} is interpreted by compiler as
    // memset(&pcb->p_s,0,sizeof(state_t)),
    // hence the inclusion of memset.h
    pcb->p_s = (state_t){0};
    pcb->start_tm = pcb->user_tm = pcb->kernel_tm = 0;

    return pcb;
}

/* PCB queue handling functions */
void mkEmptyProcQ(struct list_head *head) {
    INIT_LIST_HEAD(head);
}

bool emptyProcQ(struct list_head *head) {
    return list_empty(head);
}

void insertProcQ(struct list_head *head, pcb_t *p) {
    if (emptyProcQ(head)) {
        list_add_tail(&(p->p_next), head);
        return;
    }

    struct pcb_t *it;
    list_for_each_entry(it, head, p_next) {
        // continue until correct priority reached
        if (it->priority < p->priority) {
            list_add_tail(&(p->p_next), &(it->p_next));
            return;
        }
    }

    list_add_tail(&(p->p_next), head);
}

pcb_t *headProcQ(struct list_head *head) {
    if (list_empty(head))
        return NULL;

    return container_of(list_next(head), pcb_t, p_next);
}

pcb_t *removeProcQ(struct list_head *head) {
    pcb_t *pcb = headProcQ(head);
    if (pcb == NULL) return NULL;

    list_del(&(pcb->p_next));

    return pcb;
}

pcb_t *outProcQ(struct list_head *head, pcb_t *p) {
    struct pcb_t *iterator;

    list_for_each_entry(iterator, head, p_next) {
        if (iterator == p) {
            list_del(&(p->p_next));
            return p;
        }
    }

    return NULL;
}

/* Tree view functions */
bool emptyChild(pcb_t *this) {
    return list_empty(&(this->p_child));
}

void insertChild(pcb_t *parent, pcb_t *p) {
    list_add_tail(&(p->p_sib), &(parent->p_child));
    p->p_parent = parent;
}

pcb_t *removeChild(pcb_t *p) {
    if (list_empty(&(p->p_child)))
        return NULL;

    struct list_head *child = list_next(&(p->p_child));
    pcb_t *pcb = container_of(child, pcb_t, p_sib);

    list_del(child);

    // only for clarity, as allocPcb() is responsible
    // for restoring fields
    pcb->p_parent = NULL;

    return pcb;
}

pcb_t *outChild(pcb_t *p) {
    if (p->p_parent == NULL)
        return NULL;

    list_del(&(p->p_sib));
    p->p_parent = NULL;
    return p;
}
