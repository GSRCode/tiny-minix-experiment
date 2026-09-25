#include "prototypes.h"
void proc_b(void)
{
    kprint("Process B starting...\n");

    __asm__ volatile (
        "movl $2, %%eax\n\t"
        "movl $0, %%ebx\n\t"
        "int $0x80"
        :
        :
        : "eax", "ebx"
    );

    kprint("Process B after RECEIVE \n");


    for (;;) {
        //kprint("B ");
    }
}

