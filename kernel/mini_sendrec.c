#include "constants.h"
#include "globals.h"
#include "prototypes.h"

int mini_sendrec(struct proc *caller, int dst_nr, struct message *m_ptr)
{
    int result;

    //send part of sendrec
    caller->p_sendrec = 1;

    result = mini_send(caller, dst_nr, m_ptr);

    //SEND failed, for example because a deadlock was detected.
    if (result != 0) {
        caller->p_sendrec = 0;
        return result;
    }
    
    //SEND could not complete immediately. Caller is now blocked.
    //RECEIVE will be started when the destination eventually receives
    if (caller->p_rts_flags & SENDING) return 0;
 
    //SEND completed immediately. We can start RECEIVE right now.
    caller->p_sendrec = 0;

    result = mini_receive(caller, dst_nr, m_ptr); 

    return result;
}