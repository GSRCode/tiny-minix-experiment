#ifndef PROC_H
#define PROC_H

#include "constants.h"
#include "stackframe.h"

struct proc {
    struct stackframe p_reg;
    unsigned long p_sp; /* Saved process stack pointer. */
    int p_nr;
    int p_ticks_left;
    int p_quantum_size;
    unsigned char p_rts_flags;
    char p_name[P_NAME_LEN];
};

#endif
