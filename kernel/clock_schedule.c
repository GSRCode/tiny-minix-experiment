#include "globals.h"
#include "prototypes.h"

unsigned long clock_schedule(unsigned long current_sp)
{
    struct proc *old_proc;
    if (!need_resched) return current_sp;
    old_proc = proc_ptr;
    if (old_proc != 0) old_proc->p_sp = current_sp;
    sched();
    if (proc_ptr == 0) {
        proc_ptr = old_proc;
        need_resched = 0;
        if (old_proc != 0) old_proc->p_ticks_left = old_proc->p_quantum_size;
        return current_sp;
    }
    return proc_ptr->p_sp;
}
