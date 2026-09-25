#include "constants.h"
#include "globals.h"
#include "prototypes.h"

int mini_receive(struct proc *caller, int src_nr, struct message *m_ptr)
{
    struct proc *sender;
    struct proc *previous;
    
    int i;

    //First check whether there is a pending notification.
    //RECEIVE(ANY) accepts a notification from any process.
    //RECEIVE(src_nr) accepts only a notification from src_nr.
    
    for (i = 0; i < NR_PROCS; i++) {

        if (caller->p_pending_notify & (1U << i)) {

            if (src_nr == ANY || src_nr == i) {

                //Remove this notification from the pending notification bitmap.
                caller->p_pending_notify &= ~(1U << i);

                //Construct the notification message.
                m_ptr->m_source = i;
                m_ptr->m_type = NOTIFY_MESSAGE;
                m_ptr->m_value = 0;

                return 0;
            }
        }
    }

     
    //No matching pending notification.
    
    //walk the linked list of senders in caller->p_caller_q; and remove process src_nr (sender)
    sender = caller->p_caller_q;
    previous = 0;

    while (sender != 0) {

        if (src_nr == ANY || sender->p_nr == src_nr) { 

            if (previous == 0) caller->p_caller_q = sender->p_q_link;
            else previous->p_q_link = sender->p_q_link;

            sender->p_q_link = 0;
            *m_ptr = *(sender->p_messbuf);

            //sender not blocked
            sender->p_sendto = -1;
            sender->p_rts_flags &= ~SENDING;

            //Was this SEND part of SENDREC?
            
            if (sender->p_sendrec) {
                
                sender->p_sendrec = 0;
                
                sender->p_getfrom = caller->p_nr;
                
                //send goes from SENDING to RECEIVING and still blocked
                sender->p_rts_flags |= RECEIVING; 

                //Keep p_messbuf? yes, this is sendrec not send


            } else {
                
                //Ordinary SEND has completed.
                sender->p_messbuf = 0;

                if (sender->p_rts_flags == 0)
                    enqueue(sender); //make runnable
            }
            
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