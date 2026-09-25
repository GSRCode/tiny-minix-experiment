#include "constants.h"
#include "globals.h"
#include "prototypes.h"

int mini_send(struct proc *caller, int dst_nr, struct message *m_ptr)
{
    struct proc *dst;
    struct proc *p;

    if (dst_nr < 0 || dst_nr >= NR_PROCS)
        return -1;

    dst = &proc[dst_nr];

    if (dst->p_rts_flags & SLOT_FREE)
        return -1;

    //Is the destination already waiting to receive from this caller?
    if ((dst->p_rts_flags & RECEIVING) && 
    (dst->p_getfrom == ANY || dst->p_getfrom == caller->p_nr)) 
    {
        //send and receive has matched
        *(dst->p_messbuf) = *m_ptr;
        dst->p_getfrom = -1;
        dst->p_rts_flags &= ~RECEIVING;

        if (dst->p_rts_flags == 0) enqueue(dst);

        return 0; //sender does not block as receiver was waiting
    }

    /*
     * Caller is now blocked trying to send
     * to dst_nr.
     */
    caller->p_sendto = dst_nr;
    caller->p_messbuf = m_ptr;
    caller->p_rts_flags |= SENDING;

    /*
     * A blocked process must not remain
     * on a ready queue.
     */
    dequeue(caller);

    /*
     * Add caller to destination's sender queue.
     */
    caller->p_q_link = 0;

    if (dst->p_caller_q == 0) {
        dst->p_caller_q = caller;
    } else {
        p = dst->p_caller_q;

        while (p->p_q_link != 0)
            p = p->p_q_link;

        p->p_q_link = caller;
    }

    return 0;
}