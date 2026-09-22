#include "proc.h"

struct proc proc[NR_PROCS];
struct proc *proc_ptr;
static unsigned char proc_stack[NR_PROCS][K_STACK_SIZE];

extern void switch_context(unsigned long *old_sp,
                           unsigned long new_sp);


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
    struct stackframe *frame;

    if (nr < 0 || nr >= NR_PROCS) return;

    proc[nr].p_nr = nr;
    proc[nr].p_rts_flags = 0; //0 means runnable

    copy_name(proc[nr].p_name, name);

    frame = (struct stackframe *)
        (&proc_stack[nr][K_STACK_SIZE] -
         sizeof(struct stackframe));

    frame->edi = 0;
    frame->esi = 0;
    frame->ebp = 0;
    frame->esp_dummy = 0;

    frame->ebx = 0;
    frame->edx = 0;
    frame->ecx = 0;
    frame->eax = 0;

    frame->eip = (unsigned long)entry;
    frame->cs = 0x08;
    frame->eflags = 0x202;

    //p_sp points at the complete saved frame.
    proc[nr].p_sp = (unsigned long)frame;
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

void yield(void)
{
    struct proc *old_proc;
    struct proc *new_proc;

    old_proc = proc_ptr;

    sched(); //Select the next runnable process.

    new_proc = proc_ptr;

    if (new_proc == 0 || new_proc == old_proc) //Nothing to switch to
        return;

    /*
     * Save the old process's ESP and load
     * the new process's ESP.
     */
    switch_context(&old_proc->p_sp,
                   new_proc->p_sp);
}