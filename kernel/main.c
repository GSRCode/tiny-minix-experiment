#include "console.h"
#include "protect.h"
#include "pic.h"

extern void trigger_divide_error(void);
extern void trigger_invalid_opcode(void);
extern void trigger_general_protection(void);


void kernel_main(void)
{
    console_clear();

    kprint("Tiny MINIX starting...\n");

    /*
     * Initialize the protected-mode interrupt
     * descriptor table.
     */
    prot_init();

    kprint("IDT initialized.\n");

    pic_init();

    kprint("PIC initialized and IRQs masked.\n");

    /*
     * Deliberately generate exception vector 0.
     */
    // kprint("Triggering divide error...\n");
    // trigger_divide_error();
    
    // kprint("Triggering invalid opcode...\n");
    // trigger_invalid_opcode();

    //kprint("Triggering general protection fault...\n");
    //trigger_general_protection();


    kprint("Kernel ready.\n");


    while (1)
    {
    }
}