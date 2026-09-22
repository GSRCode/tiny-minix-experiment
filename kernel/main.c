#include "console.h"
#include "protect.h"
#include "pic.h"
#include "pit.h"
#include "clock.h"

extern void trigger_divide_error(void);
extern void trigger_invalid_opcode(void);
extern void trigger_general_protection(void);
extern void enable_interrupts(void);
extern void cpu_halt(void);


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
    pit_init(100);
    pic_unmask_irq(0);

    enable_interrupts();

    // kprint("Triggering divide error...\n");
    // trigger_divide_error();
    
    // kprint("Triggering invalid opcode...\n");
    // trigger_invalid_opcode();

    //kprint("Triggering general protection fault...\n");
    //trigger_general_protection();


    unsigned long last_second;

    last_second = 0;    

    while (1)
    {

        /*
         * Sleep until an interrupt occurs.
         *
         * At present the only enabled hardware interrupt
         * is IRQ0 from the PIT.
         */
        cpu_halt();

        /*
         * After IRQ0:
         *
         * clock_interrupt()
         *     -> clock_handler()
         *     -> clock_ticks++
         *     -> EOI
         *     -> IRETD
         *
         * Execution eventually returns here.
         */
                 
        if (clock_ticks >= last_second + 100) {

            last_second += 100;

            kprint("Clock: ");
            kprint_uint(last_second / 100);
            kprint(" seconds \n");
        }        
    }
}