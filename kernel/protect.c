#include "descriptor.h"
#include "protect.h"
#include "pic.h"

extern void divide_error(void);
extern void invalid_opcode(void);
extern void general_protection(void);
extern void clock_interrupt(void);

/*
 * Interrupt Descriptor Table. The 80386 supports 256 interrupt/exception vectors.
 */

#define IDT_SIZE 256

static struct gatedesc_s idt[IDT_SIZE];

/*
 * Pointer loaded into the CPU's IDTR register.
 */

static struct desctableptr_s idt_ptr;


/*
 * Assembly routine that executes LIDT.
 */

extern void idt_load(struct desctableptr_s *ptr);

/*
 * Build one interrupt gate in the IDT.
 *
 * This is a simplified 80386-only version of
 * MINIX 3.1.0's int_gate().
 */

static void int_gate(unsigned int vec_nr, unsigned int offset, unsigned int dpl_type)
{
    struct gatedesc_s *idp;
    
    idp = &idt[vec_nr]; /* Select the required IDT entry. */
    idp->offset_low = offset & 0xFFFF; /* Store the low 16 bits of the handler address. */
    idp->offset_high = offset >> 16; /* Store the high 16 bits of the handler address. */
    idp->selector = CS_SELECTOR; /* Handler executes using our kernel code segment. */
    idp->pad = 0; /* Reserved byte must be zero. */
    idp->p_dpl_type = dpl_type; /* Store Present, privilege and interrupt-gate type bits. */
}

/*
 * Initialize protected-mode interrupt gates.
 */

void prot_init(void)
{

    static struct gate_table_s { void (*gate)(void); unsigned char vec_nr; unsigned char privilege; }
   
    gate_table[] = {
        { divide_error, DIVIDE_VECTOR, INTR_PRIVILEGE },
        { invalid_opcode, INVALID_OPCODE_VECTOR, INTR_PRIVILEGE },
        { general_protection, GENERAL_PROTECTION_VECTOR, INTR_PRIVILEGE },
        { clock_interrupt, PIC_MASTER_OFFSET, INTR_PRIVILEGE }
    };

    unsigned int i;

    for (i = 0; i < sizeof(gate_table) / sizeof(gate_table[0]); i++)
    {
        int_gate( gate_table[i].vec_nr, (unsigned int)gate_table[i].gate,
            PRESENT | INT_GATE_TYPE | (gate_table[i].privilege << DPL_SHIFT) );
    }

    /*
     * Tell the CPU where our IDT is located.
     * The IDTR limit is the size of the table minus one.
     */

    idt_ptr.limit = sizeof(idt) - 1;

    /*
     * Because our protected-mode data segments are flat,
     * the C address of idt is also its linear address.
     */

    idt_ptr.base = (unsigned int)idt;

    /*
     * Load the CPU's IDTR register.
     */

    idt_load(&idt_ptr);
}