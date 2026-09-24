#include "globals.h"
#include "prototypes.h"

void pick_proc(void)
{
    int q;
    proc_ptr = 0;

    for (q = 0; q < NR_SCHED_QUEUES; q++) {
        if (rdy_head[q] != 0) { proc_ptr = rdy_head[q]; return; } //0 queue is most important, 1 next etc.
    }
}