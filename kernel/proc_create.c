#include "globals.h"
#include "prototypes.h"

void proc_create(int nr, const char *name, void (*entry)(void))
{
    struct stackframe *frame;
    if (nr < 0 || nr >= NR_PROCS) return;
    proc[nr].p_nr = nr;
    proc[nr].p_rts_flags = 0;
    proc[nr].p_quantum_size = DEFAULT_QUANTUM;
    proc[nr].p_ticks_left = proc[nr].p_quantum_size;
    copy_name(proc[nr].p_name, name);
    frame = (struct stackframe *)(&proc_stack[nr][K_STACK_SIZE] - sizeof(struct stackframe));
    frame->edi = 0; frame->esi = 0; frame->ebp = 0; frame->esp_dummy = 0;
    frame->ebx = 0; frame->edx = 0; frame->ecx = 0; frame->eax = 0;
    frame->eip = (unsigned long)entry;
    frame->cs = 0x08;
    frame->eflags = 0x202;
    proc[nr].p_sp = (unsigned long)frame;
}
