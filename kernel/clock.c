#include "clock.h"
#include "console.h"
#include "pic.h"

volatile unsigned long clock_ticks = 0;

/*
 * Temporary clock interrupt handler.
 *
 * For now it only proves that control can pass:
 *
 * hardware interrupt stub -> C handler -> assembly stub
 *
 * Later this function will maintain the system tick count.
 */
void clock_handler(void)
{
    clock_ticks++;

 /*
     * Signal End Of Interrupt to the master PIC.
     *
     * Without this, the PIC would consider IRQ0
     * still being serviced and would not properly
     * deliver subsequent interrupts.
     */
    pic_eoi();
}