#include "globals.h"
#include "prototypes.h"

void kernel_main(void)
{
    unsigned long last_second;
    console_clear();
    kprint("Tiny MINIX starting...\n");
    prot_init();
    kprint("IDT initialized.\n");
    pic_init();
    pit_init(100);
    proc_init();
    
    proc_create(0, "proc_a", proc_a);
    proc_create(1, "proc_b", proc_b);
    
    
    kprint("Process table initialized.\n");
    pic_unmask_irq(0);
    enable_interrupts();
    sched();
    if (proc_ptr != 0) {
        kprint("Selected process: ");
        kprint(proc_ptr->p_name);
        kprint("\n");
        restore_context(proc_ptr->p_sp);
    }
    kprint("After unmask irq and enable interrupts and restore_context ");
    last_second = 0;
    while (1) {
        cpu_halt();
        if (clock_ticks >= last_second + 100) {
            last_second += 100;
            kprint("Clock: ");
            kprint_uint(last_second / 100);
            kprint(" seconds \n");
        }
    }
}
