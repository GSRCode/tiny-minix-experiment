#include "globals.h"
#include "prototypes.h"

void proc_init(void)
{
    int i;
    unsigned char *p;
    unsigned int j;

    for (i = 0; i < NR_SCHED_QUEUES; i++) { rdy_head[i] = 0; rdy_tail[i] = 0; }
    
    for (i = 0; i < NR_PROCS; i++) {
        proc[i].p_nr = i;
        proc[i].p_rts_flags = SLOT_FREE;
        proc[i].p_name[0] = '\0';
        proc[i].p_priority = USER_Q;
        proc[i].p_nextready = 0;
        
        proc[i].p_sendto = -1;
        proc[i].p_getfrom = -1;
        proc[i].p_messbuf = 0;
        proc[i].p_caller_q = 0;
        proc[i].p_q_link = 0;
        
        proc[i].p_sendrec = 0;

        p = (unsigned char *)&proc[i].p_reg;
        for (j = 0; j < sizeof(struct stackframe); j++) p[j] = 0;
    }
}
