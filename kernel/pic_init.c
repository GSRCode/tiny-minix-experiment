#include "constants.h"
#include "prototypes.h"

void pic_init(void)
{
    outb(PIC_MASTER_COMMAND, 0x11); outb(PIC_SLAVE_COMMAND, 0x11);
    outb(PIC_MASTER_DATA, PIC_MASTER_OFFSET); outb(PIC_SLAVE_DATA, PIC_SLAVE_OFFSET);
    outb(PIC_MASTER_DATA, 0x04); outb(PIC_SLAVE_DATA, 0x02);
    outb(PIC_MASTER_DATA, 0x01); outb(PIC_SLAVE_DATA, 0x01);
    outb(PIC_MASTER_DATA, 0xFF); outb(PIC_SLAVE_DATA, 0xFF);
}
