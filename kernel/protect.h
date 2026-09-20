#ifndef PROTECT_H
#define PROTECT_H

/*
 * Constants for 80386 protected mode.
 *
 * Based on MINIX 3.1.0 protect.h, but simplified
 * for a 32-bit 80386-only kernel.
 */


/* Exception vectors. */

#define DIVIDE_VECTOR       0


/* Privilege levels. */

#define INTR_PRIVILEGE      0


/* Descriptor bits. */

#define PRESENT             0x80

#define DPL_SHIFT           5


/* 80386 32-bit interrupt gate. */

#define INT_GATE_TYPE       0x0E


/*
 * Kernel code selector.
 *
 * GDT entry 1 = kernel code segment.
 * 1 * 8 = 0x08.
 */

#define CS_SELECTOR         0x08

void prot_init(void);

#endif