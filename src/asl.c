#include "asl.h"

#include "const.h"
#include "listx.h"
#include "term.h"
#include "types_bikaya.h"

HIDDEN semd_t semd_table[MAXPROC];
HIDDEN LIST_HEAD(semdFree);
HIDDEN LIST_HEAD(asl);

/* ASL handling functions */
semd_t *getSemd(int *key) {
    semd_t *it;
    list_for_each_entry(it, &asl, s_next) {
        if (it->s_key == key)
            return it;
    }
    return NULL;
}

void initASL() {
    for (int i = 0; i < MAXPROC; i++) {
        semd_table[i].s_key = NULL;
        INIT_LIST_HEAD(&semd_table[i].s_procQ);

        list_add(&semd_table[i].s_next, &semdFree);
    }
}

int insertBlocked(int *key, pcb_t *p) {
    semd_t *semd = getSemd(key);
    if (semd == NULL) {
        if (list_empty(&semdFree))
            return TRUE;
        semd = container_of(list_next(&semdFree), semd_t, s_next);
        list_del(&semd->s_next);
        list_add(&semd->s_next, &asl);
    }

    semd->s_key = key;
    list_add_tail(&p->p_next, &semd->s_procQ);
    p->p_semkey = key;
    return FALSE;
}

pcb_t *removeBlocked(int *key) {
    semd_t *semd = getSemd(key);
    if (semd == NULL)
        return NULL;

    struct list_head *p_h = list_next(&semd->s_procQ);
    pcb_t *p = container_of(p_h, pcb_t, p_next);
    list_del(p_h);
    if (list_empty(&semd->s_procQ)) {
        list_del(&semd->s_next);
        list_add(&semd->s_next, &semdFree);
    }
    p->p_semkey = NULL;
    return p;
}

pcb_t *outBlocked(pcb_t *p) {
    semd_t *semd = getSemd(p->p_semkey);
    if (semd == NULL) return NULL;
    bool found = FALSE;
    pcb_t *it;
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
    pcb_t *it;
    list_for_each_entry(it, &semd->s_procQ, p_next) {
        if (p->p_parent == p) {
            list_del(&it->p_next);
        }
    }
    outBlocked(p);
}