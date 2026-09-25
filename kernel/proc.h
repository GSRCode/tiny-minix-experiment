#ifndef PROC_H
#define PROC_H

#include "constants.h"
#include "stackframe.h"
#include "message.h"

struct proc {
    struct stackframe p_reg;
    unsigned long p_sp; /* Saved process stack pointer. */
    int p_nr;
    int p_ticks_left;
    int p_quantum_size;
    int p_priority; //which of the 16 queues the process belongs
    struct proc *p_nextready; //p_nextready forms the linked list.
    unsigned char p_rts_flags;
    char p_name[P_NAME_LEN];
    int p_sendto; //who am I sending to?
    int p_getfrom;   // Who am I waiting to receive from? 
    struct message *p_messbuf;
    struct proc *p_caller_q; // who is waiting to send to me?
    struct proc *p_q_link;  //next sender in that queue
    int p_sendrec; //send and receive?
};

#endif
