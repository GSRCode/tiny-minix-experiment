#include "descriptor.h"
#include "protect.h"

extern void divide_error(void);
extern void invalid_opcode(void);
extern void general_protection(void);

/*
 * Interrupt Descriptor Table.
 *
 * The 80386 supports 256 interrupt/exception vectors.
 * Each entry is an 8-byte gate descriptor.
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

static void int_gate(unsigned int vec_nr,
                     unsigned int offset,
                     unsigned int dpl_type)
{
    struct gatedesc_s *idp;

    /* Select the required IDT entry. */

    idp = &idt[vec_nr];

    /* Store the low 16 bits of the handler address. */

    idp->offset_low = offset & 0xFFFF;

    /* Handler executes using our kernel code segment. */

    idp->selector = CS_SELECTOR;

    /* Reserved byte must be zero. */

    idp->pad = 0;

    /* Store Present, privilege and interrupt-gate type bits. */

    idp->p_dpl_type = dpl_type;

    /* Store the high 16 bits of the handler address. */

    idp->offset_high = offset >> 16;
}

/*
 * Initialize protected-mode interrupt gates.
 *
 * This is a simplified version of MINIX 3.1.0's prot_init().
 * For now, we install only the divide-error exception.
 */

void prot_init(void)
{
    /*
     * Each entry tells us:
     *
     *   gate       = assembly routine that handles the exception
     *   vec_nr     = IDT vector number
     *   privilege  = privilege level allowed to invoke the gate
     */

    static struct gate_table_s
    {
        void (*gate)(void);
        unsigned char vec_nr;
        unsigned char privilege;
    }
   
    gate_table[] =
    {
        {
        divide_error,
        DIVIDE_VECTOR,
        INTR_PRIVILEGE
        },

        {
        invalid_opcode,
        INVALID_OPCODE_VECTOR,
        INTR_PRIVILEGE
        },

        {
        general_protection,
        GENERAL_PROTECTION_VECTOR,
        INTR_PRIVILEGE
        }
    };
    unsigned int i;

    /*
     * Build an IDT interrupt gate for every entry
     * in gate_table.
     */

    for (i = 0;
         i < sizeof(gate_table) / sizeof(gate_table[0]);
         i++)
    {
        int_gate(
            gate_table[i].vec_nr,
            (unsigned int)gate_table[i].gate,
            PRESENT |
            INT_GATE_TYPE |
            (gate_table[i].privilege << DPL_SHIFT)
        );
    }

    /*
     * Tell the CPU where our IDT is located.
     *
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