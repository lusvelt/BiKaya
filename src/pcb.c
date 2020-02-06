#include "pcb.h"

#include "const.h"
#include "memset.h"

HIDDEN pcb_t pcbTable[MAXPROC];
HIDDEN struct list_head *pcbFree;

/* PCB free list handling functions */
void initPcbs(void) {
    int i;
    INIT_LIST_HEAD(pcbFree);

    for (i = 0; i < MAXPROC; i++) {
        list_add(&(pcbTable[i].p_next), pcbFree);
    }
}

void freePcb(pcb_t *p) {
    list_add(&(p->p_next), pcbFree);
}

pcb_t *allocPcb(void) {
    struct list_head *next = list_next(pcbFree);

    if (next == NULL)
        return NULL;

    list_del(next);
    pcb_t *pcb = container_of(next, pcb_t, p_next);

    INIT_LIST_HEAD(&(pcb->p_child));
    INIT_LIST_HEAD(&(pcb->p_next));
    INIT_LIST_HEAD(&(pcb->p_sib));
    pcb->p_parent = NULL;
    pcb->p_semkey = NULL;
    pcb->priority = 0;
    pcb->p_s = (state_t){0};

    return pcb;
}

/* PCB queue handling functions */
void mkEmptyProcQ(struct list_head *head) {}
int emptyProcQ(struct list_head *head) {}
void insertProcQ(struct list_head *head, pcb_t *p) {}
pcb_t *headProcQ(struct list_head *head) {}

pcb_t *removeProcQ(struct list_head *head) {}
pcb_t *outProcQ(struct list_head *head, pcb_t *p) {}

/* Tree view functions */
int emptyChild(pcb_t *this) {
}

void insertChild(pcb_t *prnt, pcb_t *p) {
}

pcb_t *removeChild(pcb_t *p) {
}

pcb_t *outChild(pcb_t *p) {
}