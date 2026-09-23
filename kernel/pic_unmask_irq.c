#include "constants.h"
#include "prototypes.h"

void pic_unmask_irq(unsigned int irq)
{
    unsigned char mask;
    if (irq >= 8) return;
    mask = inb(PIC_MASTER_DATA);
    mask &= ~(1 << irq);
    outb(PIC_MASTER_DATA, mask);
}
