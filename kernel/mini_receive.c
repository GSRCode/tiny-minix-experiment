#include "constants.h"
#include "globals.h"
#include "prototypes.h"

int mini_receive(struct proc *caller, int src_nr, struct message *m_ptr)
{
    struct proc *sender;
    struct proc *previous;
    struct proc *p;

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

                //since sender will receive back during SENDREC, 
                // I (receiver) need to put myself on sender's queue 
                //no deadlock is caused as sender is ~SENDING but  |= RECEIVING
                //no not required as sender is wating in SENDREC mode
                /*
                if (sender->p_caller_q == 0) { sender->p_caller_q = caller;} 
                else {
                    p = sender->p_caller_q;
                    while (p->p_q_link != 0) p = p->p_q_link;
                    p->p_q_link = caller;
                }
                */

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