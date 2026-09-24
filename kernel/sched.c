#include "globals.h"
#include "prototypes.h"

void sched(void)
{
    struct proc *old_proc;
    old_proc = proc_ptr;

    if (old_proc != 0 && old_proc->p_rts_flags == 0) {
        dequeue(old_proc);
        old_proc->p_ticks_left = old_proc->p_quantum_size;
        enqueue(old_proc);
    }

    pick_proc();

    need_resched = 0;
}
