#include "globals.h"
#include "prototypes.h"

void proc_init(void)
{
    int i;
    unsigned char *p;
    unsigned int j;
    proc_ptr = 0;
    for (i = 0; i < NR_PROCS; i++) {
        proc[i].p_nr = i;
        proc[i].p_rts_flags = SLOT_FREE;
        proc[i].p_name[0] = '\0';
        p = (unsigned char *)&proc[i].p_reg;
        for (j = 0; j < sizeof(struct stackframe); j++) p[j] = 0;
    }
}
