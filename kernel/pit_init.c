#include "constants.h"
#include "prototypes.h"

void pit_init(unsigned int frequency)
{
    unsigned int divisor = PIT_FREQUENCY / frequency;
    outb(PIT_COMMAND, PIT_COMMAND_MODE3);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}
