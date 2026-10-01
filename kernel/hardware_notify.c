#include "constants.h"
#include "globals.h"
#include "prototypes.h"

int hardware_notify(int dst_nr)
{
    struct proc *dst;

    if (dst_nr < 0 || dst_nr >= NR_PROCS) return -1;

    dst = &proc[dst_nr];

    if (dst->p_rts_flags & SLOT_FREE) return -1;

    //Is the destination already waiting for HARDWARE or ANY?
    if ((dst->p_rts_flags & RECEIVING) &&
        (dst->p_getfrom == ANY ||
         dst->p_getfrom == HARDWARE)) {

        dst->p_messbuf->m_source = HARDWARE;
        dst->p_messbuf->m_type = NOTIFY_MESSAGE;
        dst->p_messbuf->m_value = 0;

        dst->p_getfrom = NONE;
        dst->p_messbuf = 0;
        dst->p_rts_flags &= ~RECEIVING;

        if (dst->p_rts_flags == 0) enqueue(dst);
    }
    else {
        
        //not waiting
        dst->p_pending_hardware = 1;
    }

    return 0;
}