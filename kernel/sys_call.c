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
        result = mini_send(caller, (int) frame->ebx, (struct message *)frame->ecx);

        //because sys_call's return value will be in eax which will be a stack pointer for context switch
        frame->eax = result; 

        // if mini_send() blocked the caller choose another process
        if (caller->p_rts_flags != 0) pick_proc();
    
    } else if (frame->eax == RECEIVE){
        mini_receive(caller, (int) frame->ebx, (struct message *)frame->ecx);

        // if mini_receive() blocked the caller choose another process
        if (caller->p_rts_flags != 0) pick_proc();

    } else if (frame->eax == SENDREC) {
        result = mini_sendrec(caller, (int) frame->ebx, (struct message *) frame->ecx);
        
        frame->eax = result;
        
        //if mini_sendrec blocked caller, choose another proc
        if (caller->p_rts_flags != 0) pick_proc();
    }    

    //Return the stack pointer of the process that should run
    return proc_ptr->p_sp;
}