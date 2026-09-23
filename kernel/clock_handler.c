#include "globals.h"
#include "prototypes.h"

void clock_handler(void)
{
    clock_ticks++;
    sched_tick();
    pic_eoi();
}
