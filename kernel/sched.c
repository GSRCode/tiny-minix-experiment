#include "globals.h"
#include "prototypes.h"

void sched(void)
{
    int start; int i; int nr;
    if (proc_ptr == 0) start = 0;
    else start = proc_ptr->p_nr + 1;
    for (i = 0; i < NR_PROCS; i++) {
        nr = (start + i) % NR_PROCS;
        if (proc[nr].p_rts_flags == 0) {
            proc_ptr = &proc[nr];
            proc_ptr->p_ticks_left = proc_ptr->p_quantum_size;
            need_resched = 0;
            return;
        }
    }
    proc_ptr = 0;
}
