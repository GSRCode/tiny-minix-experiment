#ifndef PROTECT_H
#define PROTECT_H

/* Exception vectors. */
#define DIVIDE_VECTOR       0
#define INVALID_OPCODE_VECTOR  6
#define GENERAL_PROTECTION_VECTOR  13


/* Privilege levels. */
#define INTR_PRIVILEGE      0


/* Descriptor bits. */
#define PRESENT             0x80
#define DPL_SHIFT           5


/* 80386 32-bit interrupt gate. */
#define INT_GATE_TYPE       0x0E

/*
 * Kernel code selector. GDT entry 1 = kernel code segment.
 */

#define CS_SELECTOR         0x08

void prot_init(void);

#endif