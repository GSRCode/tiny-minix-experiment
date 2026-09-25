#include "constants.h"
#include "globals.h"
#include "prototypes.h"

int mini_receive(struct proc *caller, int src_nr, struct message *m_ptr)
{
    struct proc *sender;
    struct proc *previous;

    //walk the linked list of senders in caller->p_caller_q; and remove process src_nr (sender)
    sender = caller->p_caller_q;
    previous = 0;

    while (sender != 0) {

        if (src_nr == ANY || sender->p_nr == src_nr) { 

            if (previous == 0) caller->p_caller_q = sender->p_q_link;
            else previous->p_q_link = sender->p_q_link;

            sender->p_q_link = 0;
            *m_ptr = *(sender->p_messbuf);
            sender->p_messbuf = 0;

            //sender not blocked
            sender->p_sendto = -1;
            sender->p_rts_flags &= ~SENDING;

            //make sender runnable 
            if (sender->p_rts_flags == 0)
                enqueue(sender);

            return 0;
        }

        previous = sender;
        sender = sender->p_q_link;
    }

    //block waiting for caller
    caller->p_getfrom = src_nr;
    caller->p_messbuf = m_ptr;
    caller->p_rts_flags |= RECEIVING;
    dequeue(caller);
    
    return 0;
}