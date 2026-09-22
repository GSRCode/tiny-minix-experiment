#include "pit.h"

/*
 * PIT I/O ports.
 *
 * Channel 0 is connected to IRQ0 and is used
 * as the system timer.
 */
#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43

/*
 * PIT command:
 * 00 = select channel 0
 * 11 = access mode: low byte followed by high byte
 * 011 = mode 3 (square wave generator)
 * 0 = binary counting
 * Binary:
 *     00 11 011 0
 * Hex:
 *     0x36
 */
#define PIT_COMMAND_MODE3 0x36

extern void outb(unsigned short port, unsigned char value);

void pit_init(unsigned int frequency)
{
    unsigned int divisor;

    //get the divisor for requested frequency
    divisor = PIT_FREQUENCY / frequency;

    /*
     * Configure channel 0:
     * low byte + high byte
     * mode 3
     * binary counter
     */
    outb(PIT_COMMAND, PIT_COMMAND_MODE3);

    //The PIT counter is 16 bits. Write the low byte first...
    outb(PIT_CHANNEL0, divisor & 0xFF);

    //...followed by the high byte.
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}