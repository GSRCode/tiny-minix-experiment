#include "proc.h"

struct proc proc[NR_PROCS];
struct proc *proc_ptr;
static unsigned char proc_stack[NR_PROCS][K_STACK_SIZE];

static void copy_name(char *dest, const char *src)
{
    int i; i = 0;
    while (src[i] != '\0' && i < P_NAME_LEN - 1) { dest[i] = src[i]; i++; }
    dest[i] = '\0';
}

void proc_init(void)
{
    int i;
    unsigned char *p;
    unsigned int j;

    proc_ptr = 0; 

    for (i = 0; i < NR_PROCS; i++) { proc[i].p_nr = i; proc[i].p_rts_flags = SLOT_FREE; 
        proc[i].p_name[0] = '\0';

        //Clear the saved processor state.
        p = (unsigned char *)&proc[i].p_reg;

        for (j = 0; j < sizeof(struct stackframe); j++)
            p[j] = 0;
    }
}

void proc_create(int nr, const char *name, void (*entry)(void))
{
    unsigned long stack_top;

    if (nr < 0 || nr >= NR_PROCS) return;

    proc[nr].p_nr = nr;
    proc[nr].p_rts_flags = 0; //0 means runnable

    copy_name(proc[nr].p_name, name);

    //Stack grows downward on x86. Start ESP at the top of this process's private stack
    stack_top = (unsigned long)&proc_stack[nr][K_STACK_SIZE];

    proc[nr].p_reg.esp = stack_top;

    //execution should begin at its entry function
    proc[nr].p_reg.eip = (unsigned long)entry;

    //current GDT uses selector 0x08 for cs
    proc[nr].p_reg.cs = 0x08;

    //Start with interrupts enabled. Bit 1 is always set and IF is bit 9.
    proc[nr].p_reg.eflags = 0x202;
}

void sched(void)
{
    int start; int i; int nr;

    if (proc_ptr == 0) start = 0;
    else start = proc_ptr->p_nr + 1;

    //search table for next runnable process, round robin style
    for (i = 0; i < NR_PROCS; i++) {
        nr = (start + i) % NR_PROCS;

        if (proc[nr].p_rts_flags == 0) {
            proc_ptr = &proc[nr];
            return;
        }
    }

    //no runnable process exists 
    proc_ptr = 0;
}