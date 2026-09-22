#include "io.h"
#include "pic.h"


void pic_init(void)
{
    /*
     * ICW1
     *
     * 0x11:
     *
     * bit 4 = 1  -> begin initialization
     * bit 0 = 1  -> ICW4 will follow
     */

    outb(PIC_MASTER_COMMAND, 0x11);
    outb(PIC_SLAVE_COMMAND,  0x11);


    /*
     * ICW2
     *
     * Remap the interrupt vectors.
     *
     * Master:
     *
     * IRQ0 -> 0x20
     * ...
     * IRQ7 -> 0x27
     *
     * Slave:
     *
     * IRQ8  -> 0x28
     * ...
     * IRQ15 -> 0x2F
     */

    outb(PIC_MASTER_DATA, PIC_MASTER_OFFSET);
    outb(PIC_SLAVE_DATA,  PIC_SLAVE_OFFSET);


    /*
     * ICW3
     *
     * Slave PIC is connected to IRQ2 of master.
     *
     * Master gets a bit mask:
     *
     *     00000100b = 0x04
     *
     * Slave gets its cascade identity:
     *
     *     IRQ2 = 0x02
     */

    outb(PIC_MASTER_DATA, 0x04);
    outb(PIC_SLAVE_DATA,  0x02);


    /*
     * ICW4
     *
     * Select 8086/88 mode.
     */

    outb(PIC_MASTER_DATA, 0x01);
    outb(PIC_SLAVE_DATA,  0x01);


    /*
     * IMPORTANT:
     *
     * Mask all IRQ lines for now.
     *
     * A set bit means the corresponding IRQ is disabled.
     *
     * 0xFF = 11111111b
     *
     * We do NOT want hardware interrupts reaching the CPU
     * until the appropriate IDT handlers exist.
     */

    outb(PIC_MASTER_DATA, 0xFF);
    outb(PIC_SLAVE_DATA,  0xFF);
}

/*
 * Tell the master 8259A PIC that the current
 * hardware interrupt has been serviced.
 *
 * For now Tiny MINIX only handles IRQ0, which
 * belongs to the master PIC.
 */
void pic_eoi(void)
{
    outb(PIC_MASTER_COMMAND, PIC_EOI);
}

void pic_unmask_irq(unsigned int irq)
{
    unsigned char mask;

    /*
     * For now Tiny MINIX supports unmasking only
     * IRQs belonging to the master PIC.
     */
    if (irq >= 8)
        return;

    /*
     * Read the current interrupt mask.
     */
    mask = inb(PIC_MASTER_DATA);

    /*
     * A mask bit of:
     *
     *     1 = IRQ disabled
     *     0 = IRQ enabled
     *
     * Clear the bit corresponding to this IRQ.
     */
    mask &= ~(1 << irq);

    /*
     * Write the new mask back to the PIC.
     */
    outb(PIC_MASTER_DATA, mask);
}