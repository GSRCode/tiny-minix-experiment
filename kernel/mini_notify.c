#include "constants.h"
#include "globals.h"
#include "prototypes.h"

int mini_notify(struct proc *caller, int dst_nr)
{
    struct proc *dst;

    if (dst_nr < 0 || dst_nr >= NR_PROCS) return -1;

    dst = &proc[dst_nr];

    if (dst->p_rts_flags & SLOT_FREE) return -1;

    //Is destination already waiting for a message from us or ANY?
    if ((dst->p_rts_flags & RECEIVING) &&
        (dst->p_getfrom == ANY ||
         dst->p_getfrom == caller->p_nr)) {

        //Build notification directly in receiver's message buffer.
        dst->p_messbuf->m_source = caller->p_nr;
        dst->p_messbuf->m_type = NOTIFY_MESSAGE;
        dst->p_messbuf->m_value = 0;

        dst->p_getfrom = -1;
        //this does not mean messbuf is deleted, 
        // just means process structure need not hold pointer to message buffer anymore
        dst->p_messbuf = 0; 
        dst->p_rts_flags &= ~RECEIVING;

        if (dst->p_rts_flags == 0)
            enqueue(dst);
    } 
    else {
        
        //Destination is not ready - Remember this notification.
        dst->p_pending_notify |= (1U << caller->p_nr);
    }

    
    //NOTIFY never blocks the caller.For now, if destination wasn't
    //receiving, the notification is lost.

    return 0;
}