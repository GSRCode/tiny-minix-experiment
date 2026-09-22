#include "clock.h"
#include "pic.h"

volatile unsigned long clock_ticks = 0;

void clock_handler(void)
{
    clock_ticks++;

    pic_eoi(); /* Signal End Of Interrupt to the master PIC. Without this, the PIC would consider IRQ0 sill serviced */
}