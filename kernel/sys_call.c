#include "prototypes.h"
#include "stackframe.h"
#include "globals.h"

unsigned long sys_call(struct stackframe *frame)
{
    struct proc *caller;

    caller = proc_ptr;

    caller->p_sp = (unsigned long) frame;
    if (frame->eax == SEND) {
        mini_send(caller, (int) frame->ebx, (struct message *)frame->ecx);

        // if mini_send() blocked the caller choose another process
        if (caller->p_rts_flags != 0) {
            pick_proc();
        }
    } else if (frame->eax == RECEIVE){
        mini_receive(caller, (int) frame->ebx, (struct message *)frame->ecx);

        // if mini_send() blocked the caller choose another process
        if (caller->p_rts_flags != 0) {
            pick_proc();
        }

    }

    /*
     * Return the stack pointer of the process
     * that should leave the kernel.
     */
    return proc_ptr->p_sp;
}