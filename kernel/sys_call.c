#include "constants.h"
#include "prototypes.h"
#include "stackframe.h"
#include "globals.h"

unsigned long sys_call(struct stackframe *frame)
{
    struct proc *caller;
    int result;

    caller = proc_ptr;

    caller->p_sp = (unsigned long) frame;

    if (frame->eax == SEND) {

        frame->eax = mini_send(caller, (int) frame->ebx, (struct message *) frame->ecx);

        if (caller->p_rts_flags != 0) pick_proc();

    } else if (frame->eax == RECEIVE) {

        frame->eax = mini_receive(caller, (int) frame->ebx, (struct message *) frame->ecx);

        if (caller->p_rts_flags != 0) pick_proc();

    } else if (frame->eax == SENDREC) {

        frame->eax = mini_sendrec(caller, (int) frame->ebx, (struct message *) frame->ecx);

        if (caller->p_rts_flags != 0) pick_proc();

    } else if (frame->eax == NOTIFY) {

        frame->eax = mini_notify(caller, (int) frame->ebx);

    } else if (frame->eax == SYS_GETUPTIME) {

        frame->eax = system_call(SYS_GETUPTIME);
    }

    return proc_ptr->p_sp;
}