#include "constants.h"
#include "globals.h"
#include "prototypes.h"

int deadlock(int src_nr, int dst_nr)
{
    struct proc *p;

    p = &proc[dst_nr];

    //walk through process table and see if anywhere down the line destination process
    //is trying to send do source process
    while (p->p_rts_flags & SENDING) {

        if (p->p_sendto == src_nr) return 1;

        //follow chain see if A->B, B->C and C->A etc.
        p = &proc[p->p_sendto];
    }

    return 0;
}