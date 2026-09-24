#include "constants.h"
#include "globals.h"
#include "prototypes.h"

void prot_init(void)
{
    struct gate_table_s { void (*gate)(void); unsigned char vec_nr; unsigned char privilege; };
    struct gate_table_s gate_table[] = {
        { divide_error, DIVIDE_VECTOR, INTR_PRIVILEGE },
        { invalid_opcode, INVALID_OPCODE_VECTOR, INTR_PRIVILEGE },
        { general_protection, GENERAL_PROTECTION_VECTOR, INTR_PRIVILEGE },
        { clock_interrupt, PIC_MASTER_OFFSET, INTR_PRIVILEGE },
        { ipc_entry, SYS386_VECTOR, INTR_PRIVILEGE}
    };
    unsigned int i;
    for (i = 0; i < sizeof(gate_table) / sizeof(gate_table[0]); i++) {
        int_gate(gate_table[i].vec_nr, (unsigned int)gate_table[i].gate,
                 PRESENT | INT_GATE_TYPE | (gate_table[i].privilege << DPL_SHIFT));
    }
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (unsigned int)idt;
    idt_load(&idt_ptr);
}
