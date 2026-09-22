#ifndef PROC_H
#define PROC_H


#define NR_PROCS 4
#define SLOT_FREE 0x01
#define P_NAME_LEN 16
#define K_STACK_SIZE 1024
#define DEFAULT_QUANTUM 50

#include "stackframe.h"

extern volatile int need_resched;

struct proc {
    struct stackframe p_reg;
    unsigned long p_sp; //temporary (?) varialbe to store process stack pointer
    int p_nr; //Process number

    int p_ticks_left; // Number of clock ticks remaining in the current scheduling quantum.
    int p_quantum_size; // Size of a complete scheduling quantum   

    unsigned char p_rts_flags; //Runtime status flags
    char p_name[P_NAME_LEN]; //Process name.
};

/* Process table.*/
extern struct proc proc[NR_PROCS];
extern struct proc *proc_ptr; //pointer of process currently selected to run

void proc_init(void); /* Initialize the process table.*/
void proc_create(int nr, const char *name, void (*entry)(void));
void sched(void);
void yield(void);
void sched_tick(void);
unsigned long clock_schedule(unsigned long current_sp);

#endif


