#ifndef PIC_H
#define PIC_H

/* 8259A I/O ports */

#define PIC_MASTER_COMMAND  0x20
#define PIC_MASTER_DATA     0x21

#define PIC_SLAVE_COMMAND   0xA0
#define PIC_SLAVE_DATA      0xA1


/* Interrupt vector offsets */

#define PIC_MASTER_OFFSET   0x20
#define PIC_SLAVE_OFFSET    0x28


void pic_init(void);

#endif