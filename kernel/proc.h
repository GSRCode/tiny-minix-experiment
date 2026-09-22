#ifndef PROC_H
#define PROC_H


#define NR_PROCS 4
#define SLOT_FREE 0x01
#define P_NAME_LEN 16
#define K_STACK_SIZE 1024

#include "stackframe.h"

struct proc {
    struct stackframe p_reg;
    int p_nr; //Process number
    unsigned char p_rts_flags; //Runtime status flags
    char p_name[P_NAME_LEN]; //Process name.
};

/* Process table.*/
extern struct proc proc[NR_PROCS];
extern struct proc *proc_ptr; //pointer of process currently selected to run

void proc_init(void); /* Initialize the process table.*/
void proc_create(int nr, const char *name, void (*entry)(void));
void sched(void);

#endif


