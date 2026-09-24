#include "prototypes.h"
#include "globals.h"
void proc_a(void)
{
    kprint("Process A before SEND \n");

    __asm__ volatile (
        "movl $1, %%eax\n\t"
        "movl $1, %%ebx\n\t"
        "int $0x80"
        :
        :
        : "eax", "ebx"
    );

    /*
     * We should NOT see this yet.
     *
     * A is blocked because B has not implemented
     * receive().
     */
    kprint("Process A after SEND \n");

    for (;;) {
    }
}
