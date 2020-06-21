/*****************************************************************************
 * Copyright 2004, 2005 Michael Goldweber, Davide Brini.                     *
 * Modified 2012 by Marco Di Felice                                          *
 * Modified 2018 by Renzo Davoli
 *                                                                           *
 * This file is part of BIKAYA.                                              *
 *                                                                           *
 * bikaya is free software;you can redistribute it and/or modify it under the*
 * terms of the GNU General Public License as published by the Free Software *
 * Foundation; either version 2 of the License, or (at your option) any      *
 * later version.                                                            *
 * This program is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General *
 * Public License for more details.                                          *
 * You should have received a copy of the GNU General Public License along   *
 * with this program; if not, write to the Free Software Foundation, Inc.,   *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.                  *
 *****************************************************************************/

/*********************************P1TEST.C*******************************
 *
 *	Test program for the modules ASL and PCB (phase 1).
 *
 *	Produces progress messages on terminal 0 in addition
 *		to the array ``okbuf[]''
 *		Error messages will also appear on terminal 0 in
 *		addition to the array ``errbuf[]''.
 *
 *		Aborts as soon as an error is detected.
 *
 *
 */

#include "const.h"
#include "listx.h"
#include "terminal.h"

#ifdef TARGET_UMPS
#include "umps/arch.h"
#include "umps/libumps.h"
#endif
#ifdef TARGET_UARM
#include "uarm/arch.h"
#include "uarm/libuarm.h"
#endif

#include "asl.h"
#include "pcb.h"

#define MAXSEM MAXPROC

#define MAX_PCB_PRIORITY 10
#define MIN_PCB_PRIORITY 0
#define DEFAULT_PCB_PRIORITY 5

char okbuf[2048]; /* sequence of progress messages */
char errbuf[128]; /* contains reason for failing */
char msgbuf[128]; /* nonrecoverable error message before shut down */

int onesem;
pcb_t *procp[MAXPROC], *p, *q, *maxproc, *minproc, *proc;
semd_t *semd[MAXSEM];
int sem[MAXSEM + 1];

struct list_head qa;
char *mp = okbuf;

#define ST_READY 1
#define ST_BUSY 3
#define ST_TRANSMITTED 5

#define CMD_ACK 1
#define CMD_TRANSMIT 2

#define CHAR_OFFSET 8
#define TERM_STATUS_MASK 0xFF

/******************************************************************************
 * I/O Routines to write on a terminal
 ******************************************************************************/

/* This function returns the terminal transmitter status value given its address */
static unsigned int tx_status(termreg_t *tp) {
    return ((tp->transm_status) & TERM_STATUS_MASK);
}

/* This function prints a string on specified terminal and returns TRUE if
 * print was successful, FALSE if not   */
unsigned int termprint(char *str, unsigned int term) {
    termreg_t *term_reg;

    unsigned int stat;
    unsigned int cmd;

    unsigned int error = FALSE;

    if (term < DEV_PER_INT) {
        term_reg = (termreg_t *)DEV_REG_ADDR(IL_TERMINAL, term);

        /* test device status */
        stat = tx_status(term_reg);
        if ((stat == ST_READY) || (stat == ST_TRANSMITTED)) {
            /* device is available */

            /* print cycle */
            while ((*str != '\0') && (!error)) {
                cmd = (*str << CHAR_OFFSET) | CMD_TRANSMIT;
                term_reg->transm_command = cmd;

                /* busy waiting */
                while ((stat = tx_status(term_reg)) == ST_BUSY)
                    ;

                /* end of wait */
                if (stat != ST_TRANSMITTED) {
                    error = TRUE;
                } else {
                    /* move to next char */
                    str++;
                }
            }
        } else {
            /* device is not available */
            error = TRUE;
        }
    } else {
        /* wrong terminal device number */
        error = TRUE;
    }

    return (!error);
}

/* This function places the specified character string in okbuf and
 *	causes the string to be written out to terminal0 */
void addokbuf(char *strp) {
    termprint(strp, 0);
}

/* This function places the specified character string in errbuf and
 *	causes the string to be written out to terminal0.  After this is done
 *	the system shuts down with a panic message */
void adderrbuf(char *strp) {
    termprint(strp, 0);

    PANIC();
}

/******************************************************************************
 * Main Test File
 ******************************************************************************/

int main(void) {
    int i;

    pcb_init();
    addokbuf("Initialized Process Control Blocks   \n");

    /* Check pcb_alloc */
    for (i = 0; i < MAXPROC; i++) {
        if ((procp[i] = pcb_alloc()) == NULL)
            adderrbuf("pcb_alloc(): unexpected NULL   ");
    }

    if (pcb_alloc() != NULL) {
        adderrbuf(" ERROR: pcb_alloc(): allocated more than MAXPROC entries   ");
    }
    addokbuf(" pcb_alloc test OK   \n");

    /* Return the last 10 entries back to free list */
    for (i = 10; i < MAXPROC; i++)
        pcb_free(procp[i]);

    addokbuf(" Added 10 entries to the free PCB list   \n");

    /* Create a 10-element process queue */
    INIT_LIST_HEAD(&qa);

    if (!pcb_is_queue_empty(&qa))
        adderrbuf("ERROR: pcb_is_queue_empty(qa): unexpected FALSE   ");

    addokbuf("Testing pcb_insert_in_queue ...   \n");

    for (i = 0; i < 10; i++) {
        if ((q = pcb_alloc()) == NULL)
            adderrbuf("ERROR: pcb_alloc(): unexpected NULL while insert   ");
        switch (i) {
            case 3:
                q->priority = DEFAULT_PCB_PRIORITY;
                proc = q;
                break;
            case 4:
                q->priority = MAX_PCB_PRIORITY;
                maxproc = q;
                break;
            case 5:
                q->priority = MIN_PCB_PRIORITY;
                minproc = q;
                break;
            default:
                q->priority = DEFAULT_PCB_PRIORITY;
                break;
        }
        pcb_insert_in_queue(&qa, q);
    }

    addokbuf("Test pcb_insert_in_queue: OK. Inserted 10 elements \n");

    if (pcb_is_queue_empty(&qa))
        adderrbuf("ERROR: pcb_is_queue_empty(qa): unexpected TRUE");

    /* Check pcb_find_and_remove and pcb_queue_head */
    if (pcb_queue_head(&qa) != maxproc)
        adderrbuf("ERROR: pcb_queue_head(qa) failed   ");

    /* Removing an element from ProcQ */
    q = pcb_find_and_remove(&qa, proc);
    if ((q == NULL) || (q != proc))
        adderrbuf("ERROR: pcb_find_and_remove(&qa, proc) failed to remove the entry   ");
    pcb_free(q);

    /* Removing the first element from ProcQ */
    q = pcb_remove_from_queue(&qa);
    if (q == NULL || q != maxproc)
        adderrbuf("ERROR: pcb_remove_from_queue(&qa, midproc) failed to remove the elements in the right order   ");
    pcb_free(q);

    /* Removing other 7 elements  */
    addokbuf(" Testing pcb_remove_from_queue ...   \n");
    for (i = 0; i < 7; i++) {
        if ((q = pcb_remove_from_queue(&qa)) == NULL)
            adderrbuf("pcb_remove_from_queue(&qa): unexpected NULL   ");
        pcb_free(q);
    }

    // Removing the last element
    q = pcb_remove_from_queue(&qa);
    if (q != minproc)
        adderrbuf("ERROR: pcb_remove_from_queue(): failed on last entry   ");
    pcb_free(q);

    if (pcb_remove_from_queue(&qa) != NULL)
        adderrbuf("ERROR: pcb_remove_from_queue(&qa): removes too many entries   ");

    if (!pcb_is_queue_empty(&qa))
        adderrbuf("ERROR: pcb_is_queue_empty(qa): unexpected FALSE   ");

    addokbuf(" Test pcb_insert_in_queue(), pcb_remove_from_queue() and pcb_is_queue_empty(): OK   \n");
    addokbuf(" Test process queues module: OK      \n");

    addokbuf(" Testing process trees...\n");

    if (!pcb_has_no_children(procp[2]))
        adderrbuf("ERROR: pcb_has_no_children: unexpected FALSE   ");

    /* make procp[1],procp[2],procp[3], procp[7] children of procp[0] */
    addokbuf("Inserting...   \n");
    pcb_insert_child(procp[0], procp[1]);
    pcb_insert_child(procp[0], procp[2]);
    pcb_insert_child(procp[0], procp[3]);
    pcb_insert_child(procp[0], procp[7]);
    addokbuf("Inserted 4 children of pcb0  \n");

    /* make procp[8],procp[9] children of procp[7] */
    pcb_insert_child(procp[7], procp[8]);
    pcb_insert_child(procp[7], procp[9]);
    addokbuf("Inserted 2 children of pcb7  \n");

    if (pcb_has_no_children(procp[0]))
        adderrbuf("ERROR: pcb_has_no_children(procp[0]): unexpected TRUE   ");

    if (pcb_has_no_children(procp[7]))
        adderrbuf("ERROR: pcb_has_no_children(procp[7]): unexpected TRUE   ");

    /* Check pcb_find_and_remove_child */
    q = pcb_find_and_remove_child(procp[1]);
    if (q == NULL || q != procp[1])
        adderrbuf("ERROR: pcb_find_and_remove_child(procp[1]) failed ");

    q = pcb_find_and_remove_child(procp[8]);
    if (q == NULL || q != procp[8])
        adderrbuf("ERROR: pcb_find_and_remove_child(procp[8]) failed ");

    /* Check pcb_remove_child */
    q = pcb_remove_child(procp[0]);
    if (q == NULL || q != procp[2])
        adderrbuf("ERROR: pcb_remove_child(procp[0]) failed ");

    q = pcb_remove_child(procp[7]);
    if (q == NULL || q != procp[9])
        adderrbuf("ERROR: pcb_remove_child(procp[7]) failed ");

    q = pcb_remove_child(procp[0]);
    if (q == NULL || q != procp[3])
        adderrbuf("ERROR: pcb_remove_child(procp[0]) failed ");

    q = pcb_remove_child(procp[0]);
    if (q == NULL || q != procp[7])
        adderrbuf("ERROR: pcb_remove_child(procp[0]) failed ");

    if (pcb_remove_child(procp[0]) != NULL)
        adderrbuf("ERROR: pcb_remove_child(): removes too many children   ");

    if (!pcb_has_no_children(procp[0]))
        adderrbuf("ERROR: pcb_has_no_children(procp[0]): unexpected FALSE   ");

    addokbuf("Test: pcb_insert_child(), pcb_remove_child() and pcb_has_no_children() OK   \n");
    addokbuf("Testing process tree module OK      \n");

    pcb_free(procp[0]);
    pcb_free(procp[1]);
    pcb_free(procp[2]);
    pcb_free(procp[3]);
    pcb_free(procp[4]);
    pcb_free(procp[5]);
    pcb_free(procp[6]);
    pcb_free(procp[7]);
    pcb_free(procp[8]);
    pcb_free(procp[9]);

    /* check ASL */
    asl_init();
    addokbuf("Initializing active semaphore list   \n");

    /* check asl_remove_blocked and asl_insert_blocked */
    addokbuf(" Test asl_insert_blocked(): test #1 started  \n");
    for (i = 10; i < MAXPROC; i++) {
        procp[i] = pcb_alloc();
        if (asl_insert_blocked(&sem[i], procp[i]))
            adderrbuf("ERROR: asl_insert_blocked() test#1: unexpected TRUE   ");
    }

    addokbuf("Test asl_insert_blocked(): test #2 started  \n");
    for (i = 0; i < 10; i++) {
        procp[i] = pcb_alloc();
        if (asl_insert_blocked(&sem[i], procp[i]))
            adderrbuf("ERROR:asl_insert_blocked() test #2: unexpected TRUE   ");
    }

    /* check if semaphore descriptors are returned to the free list */
    p = asl_remove_blocked(&sem[11]);
    if (asl_insert_blocked(&sem[11], p))
        adderrbuf("ERROR: asl_remove_blocked(): fails to return to free list   ");

    if (asl_insert_blocked(&sem[MAXSEM], procp[9]) == FALSE)
        adderrbuf("ERROR: asl_insert_blocked(): inserted more than MAXPROC   ");

    addokbuf("Test asl_remove_blocked(): test started   \n");
    for (i = 10; i < MAXPROC; i++) {
        q = asl_remove_blocked(&sem[i]);
        if (q == NULL)
            adderrbuf("ERROR: asl_remove_blocked(): wouldn't remove   ");
        if (q != procp[i])
            adderrbuf("ERROR: asl_remove_blocked(): removed wrong element   ");
    }

    if (asl_remove_blocked(&sem[11]) != NULL)
        adderrbuf("ERROR: asl_remove_blocked(): removed nonexistent blocked proc   ");

    addokbuf("Test asl_insert_blocked() and asl_remove_blocked() ok   \n");

    if (asl_blocked_head(&sem[11]) != NULL)
        adderrbuf("ERROR: asl_blocked_head(): nonNULL for a nonexistent queue   ");

    if ((q = asl_blocked_head(&sem[9])) == NULL)
        adderrbuf("ERROR: asl_blocked_head(1): NULL for an existent queue   ");
    if (q != procp[9])
        adderrbuf("ERROR: asl_blocked_head(1): wrong process returned   ");

    p = asl_find_and_remove_blocked(q);
    if (p != q)
        adderrbuf("ERROR: asl_find_and_remove_blocked(1): couldn't remove from valid queue   ");

    /* Creating a 2-layer tree */
    pcb_insert_child(procp[0], procp[1]);
    pcb_insert_child(procp[0], procp[2]);
    pcb_insert_child(procp[0], procp[3]);
    pcb_insert_child(procp[3], procp[4]);

    /* Testing asl_find_and_remove_blocked_child */
    asl_find_and_remove_blocked_child(procp[0]);

    if (asl_blocked_head(&sem[0]) != NULL)
        adderrbuf("ERROR: asl_find_and_remove_blocked_child(): nonNULL for a nonexistent queue (0)  ");
    if (asl_blocked_head(&sem[1]) != NULL)
        adderrbuf("ERROR: asl_find_and_remove_blocked_child(): nonNULL for a nonexistent queue (1)  ");
    if (asl_blocked_head(&sem[2]) != NULL)
        adderrbuf("ERROR: asl_find_and_remove_blocked_child(): nonNULL for a nonexistent queue  (2) ");
    if (asl_blocked_head(&sem[3]) != NULL)
        adderrbuf("ERROR: asl_find_and_remove_blocked_child(): nonNULL for a nonexistent queue (3)  ");
    if (asl_blocked_head(&sem[4]) != NULL)
        adderrbuf("ERROR: asl_find_and_remove_blocked_child(): nonNULL for a nonexistent queue (4)  ");
    if (asl_blocked_head(&sem[5]) == NULL)
        adderrbuf("ERROR: asl_find_and_remove_blocked_child(): NULL for an existent queue  (5) ");

    addokbuf("Test asl_blocked_head() and asl_find_and_remove_blocked(): OK   \n");

    addokbuf("ASL module OK   \n");
    addokbuf("So Long and Thanks for All the Fish\n");
    addokbuf("So Sad that It Should Come to This \n");

    return 0;
}
