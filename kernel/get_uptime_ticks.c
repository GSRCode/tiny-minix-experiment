#include "globals.h"
#include "prototypes.h"

unsigned long get_uptime_ticks(void)
{
    return clock_ticks;
}

unsigned long get_uptime_seconds(void)
{
    return clock_ticks / 100;
}