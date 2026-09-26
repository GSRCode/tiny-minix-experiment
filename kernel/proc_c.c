#include "prototypes.h"

void proc_c(void)
{
    int result;

    kprint("C before NOTIFY A\n");

    __asm__ volatile (
        "movl $4, %%eax\n\t"       /* NOTIFY */
        "movl $0, %%ebx\n\t"       /* destination = A */
        "int $0x80\n\t"
        "movl %%eax, %0"
        : "=r" (result)
        :
        : "eax", "ebx"
    );

    kprint("C after NOTIFY A\n");

    for (;;) {
    }
}