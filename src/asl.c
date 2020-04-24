#include "asl.h"

#include "const.h"
#include "listx.h"
#include "pcb.h"
#include "types.h"

HIDDEN semd_t semdTable[MAXPROC];
HIDDEN LIST_HEAD(semdFree);  // Dummy element of semd free list
HIDDEN LIST_HEAD(asl);       // Dummy element of Active Semaphore List

semd_t *getSemd(int *key) {
    semd_t *it;
    list_for_each_entry(it, &asl, s_next) {
        if (it->s_key == key)
            return it;
    }
    return NULL;
}

void initAsl() {
    for (int i = 0; i < MAXPROC; i++) {
        semdTable[i].s_key = NULL;
        INIT_LIST_HEAD(&semdTable[i].s_procQ);

        list_add(&semdTable[i].s_next, &semdFree);
    }
}

bool insertBlocked(int *key, pcb_t *p) {
    semd_t *semd = getSemd(key);
    if (semd == NULL) {
        if (list_empty(&semdFree))
            return TRUE;
        semd = container_of(list_next(&semdFree), semd_t, s_next);
        list_del(&semd->s_next);
        list_add(&semd->s_next, &asl);
    }

    // API given does not specify a policy for the insertion of a PCB
    // in a semaphore queue. So, we decide to use insertProcQ and
    // handling semaphore queues like processes queues.
    insertProcQ(&semd->s_procQ, p);
    semd->s_key = key;
    p->p_semkey = key;
    return FALSE;
}

pcb_t *removeBlocked(int *key) {
    semd_t *semd = getSemd(key);
    if (semd == NULL)
        return NULL;

    struct list_head *elem = list_next(&semd->s_procQ);
    pcb_t *pcb = container_of(elem, pcb_t, p_next);
    list_del(elem);

    if (list_empty(&semd->s_procQ)) {
        list_del(&semd->s_next);
        list_add(&semd->s_next, &semdFree);  // We return the semaphore to the free list
    }

    pcb->p_semkey = NULL;
    return pcb;
}

pcb_t *outBlocked(pcb_t *p) {
    semd_t *semd = getSemd(p->p_semkey);
    if (semd == NULL) {
        // PCB passed as input does not exist on the given semaphore queue
        return NULL;
    }

    pcb_t *it;
    bool found = FALSE;
    list_for_each_entry(it, &semd->s_procQ, p_next) {
        if (p == it) {
            found = TRUE;
            break;
        }
    }

    if (!found) return NULL;

    list_del(&it->p_next);

    if (list_empty(&semd->s_procQ)) {
        list_del(&semd->s_next);
        list_add(&semd->s_next, &semdFree);
    }

    return p;
}

pcb_t *headBlocked(int *key) {
    semd_t *semd = getSemd(key);
    if (semd == NULL || list_empty(&semd->s_procQ)) return NULL;

    return container_of(list_next(&semd->s_procQ), pcb_t, p_next);
}

void outChildBlocked(pcb_t *p) {
    semd_t *semd = getSemd(p->p_semkey);
    if (semd == NULL) return;

    pcb_t *it;
    list_for_each_entry(it, &p->p_child, p_sib) {
        outChildBlocked(it);
    }

    outBlocked(p);
}