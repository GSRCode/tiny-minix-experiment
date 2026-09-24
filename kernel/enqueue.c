#include "globals.h"
#include "prototypes.h"

void enqueue(struct proc *rp)
{
    int q;

    q = rp->p_priority;
    if (q < 0 || q >= NR_SCHED_QUEUES) return;

    rp->p_nextready = 0;

    if (rdy_head[q] == 0) { rdy_head[q] = rp; rdy_tail[q] = rp; } //rp will be head and tail node
    else { rdy_tail[q]->p_nextready = rp; rdy_tail[q] = rp; } //attach to the existing tail node
}