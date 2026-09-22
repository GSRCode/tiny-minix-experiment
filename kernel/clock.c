#include "clock.h"
#include "pic.h"
#include "proc.h"
#include "console.h"

volatile unsigned long clock_ticks = 0;

void clock_handler(void)
{
    //kprint("in clock_handler");
    clock_ticks++;

    sched_tick();

    pic_eoi(); /* Signal End Of Interrupt to the master PIC. Without this, the PIC would consider IRQ0 sill serviced */
}