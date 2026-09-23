#include "globals.h"
#include "prototypes.h"

void sched_tick(void)
{
    if (proc_ptr == 0) return;
    if (proc_ptr->p_ticks_left > 0) proc_ptr->p_ticks_left--;
    if (proc_ptr->p_ticks_left == 0) need_resched = 1;
}
