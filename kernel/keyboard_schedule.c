#include "globals.h"
#include "prototypes.h"

unsigned long keyboard_schedule(unsigned long current_sp)
{
    //Save the context of the process that was interrupted by the keyboard.
    if (proc_ptr != 0) proc_ptr->p_sp = current_sp;

    //if tty is ready pick highest-priority process
    pick_proc();

    //Return the stack pointer of the process that should now run.
    return proc_ptr->p_sp;
}