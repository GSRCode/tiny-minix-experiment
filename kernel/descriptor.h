#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

/*
 * 80386 interrupt/trap gate descriptor.
 *
 * This structure is based on MINIX 3.1.0's
 * struct gatedesc_s in kernel/protect.c.
 */

struct gatedesc_s
{
    unsigned short offset_low;   /* Low 16 bits of handler address. */

    unsigned short selector;     /* Code segment selector. */

    unsigned char pad;           /* Reserved byte. Must be zero. */

    unsigned char p_dpl_type;    /* Present | privilege | gate type. */

    unsigned short offset_high;  /* High 16 bits of handler address. */
};

/*
 * Descriptor-table pointer used by LGDT and LIDT.
 *
 * The 80386 expects:
 *
 *   16-bit limit
 *   32-bit base address
 */

struct desctableptr_s
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

#endif