#include "globals.h"
#include "prototypes.h"

void dequeue(struct proc *rp)
{
    int q;
    struct proc *current;
    struct proc *previous;

    q = rp->p_priority; if (q < 0 || q >= NR_SCHED_QUEUES) return;

    current = rdy_head[q]; previous = 0;

    while (current != 0) {

        if (current == rp) {

            if (previous == 0) rdy_head[q] = current->p_nextready; //first node is itself rp update rdy_head
            else previous->p_nextready = current->p_nextready;

            if (rdy_tail[q] == current) rdy_tail[q] = previous; //update the rdy_tail if rp is last one

            current->p_nextready = 0;
            return;
        }

        previous = current;
        current = current->p_nextready;
    }
}