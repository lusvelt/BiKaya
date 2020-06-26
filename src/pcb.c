#include "pcb.h"

#include "const.h"
#include "memory.h"

// only relevant to this file, so HIDDEN (static)
HIDDEN pcb_t pcbTable[MAXPROC];
HIDDEN LIST_HEAD(pcbFree);  // dummy

/* PCB free list handling functions */
void pcb_init(void) {
    int i;

    for (i = 0; i < MAXPROC; i++)
        list_add(&(pcbTable[i].p_next), &pcbFree);
}

void pcb_free(pcb_t *p) {
    list_add(&(p->p_next), &pcbFree);
}

pcb_t *pcb_alloc(void) {
    struct list_head *next = list_next(&pcbFree);

    if (next == NULL)
        return NULL;

    list_del(next);
    pcb_t *pcb = container_of(next, pcb_t, p_next);

    // Clocks are set to null so that wallclock can be set in scheduler at first activation
    memset(pcb, 0, sizeof(pcb_t));

    //initialize fields
    INIT_LIST_HEAD(&(pcb->p_child));
    INIT_LIST_HEAD(&(pcb->p_next));
    INIT_LIST_HEAD(&(pcb->p_sib));
    // pcb->p_parent = NULL;
    // pcb->p_semkey = NULL;
    // pcb->priority = 0;
    // // this {0} is interpreted by compiler as
    // // memset(&pcb->p_s,0,sizeof(state_t)),
    // // hence the inclusion of memset.h
    // pcb->p_s = (state_t){0};
    // pcb->start_tm = pcb->user_tm = pcb->kernel_tm = 0;
    // memset(pcb->exc_old_areas, 0, sizeof(state_t *) * 3);
    // memset(pcb->exc_new_areas, 0, sizeof(state_t *) * 3);

    return pcb;
}

/* PCB queue handling functions */
void pcb_make_empty_queue(struct list_head *head) {
    INIT_LIST_HEAD(head);
}

bool pcb_is_queue_empty(struct list_head *head) {
    return list_empty(head);
}

void pcb_insert_in_queue(struct list_head *head, pcb_t *p) {
    if (pcb_is_queue_empty(head)) {
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

pcb_t *pcb_queue_head(struct list_head *head) {
    if (list_empty(head))
        return NULL;

    return container_of(list_next(head), pcb_t, p_next);
}

pcb_t *pcb_remove_from_queue(struct list_head *head) {
    pcb_t *pcb = pcb_queue_head(head);
    if (pcb == NULL) return NULL;

    list_del(&(pcb->p_next));

    return pcb;
}

bool pcb_is_free(pcb_t *p) {
    struct pcb_t *iterator;

    list_for_each_entry(iterator, &pcbFree, p_next) {
        if (iterator == p) return TRUE;
    }
    return FALSE;
}

pcb_t *pcb_find_and_remove(struct list_head *head, pcb_t *p) {
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
bool pcb_has_no_children(pcb_t *this) {
    return list_empty(&(this->p_child));
}

void pcb_insert_child(pcb_t *parent, pcb_t *p) {
    list_add_tail(&(p->p_sib), &(parent->p_child));
    p->p_parent = parent;
}

pcb_t *pcb_remove_child(pcb_t *p) {
    if (list_empty(&(p->p_child)))
        return NULL;

    struct list_head *child = list_next(&(p->p_child));
    pcb_t *pcb = container_of(child, pcb_t, p_sib);

    list_del(child);

    // only for clarity, as pcb_alloc() is responsible
    // for restoring fields
    pcb->p_parent = NULL;

    return pcb;
}

pcb_t *pcb_find_and_remove_child(pcb_t *p) {
    if (p->p_parent == NULL)
        return NULL;

    list_del(&(p->p_sib));
    p->p_parent = NULL;
    return p;
}