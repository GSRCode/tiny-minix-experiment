#include "prototypes.h"
#include "message.h"
#include "globals.h"

void proc_a(void)
{
    
    int result;

    kprint("A before NOTIFY\n");

    __asm__ volatile (
    "movl $4, %%eax\n\t"
    "movl $1, %%ebx\n\t"
    "int $0x80\n\t"
    "movl %%eax, %0"
    : "=r" (result)
    :
    : "eax", "ebx"
    );

    kprint("A after NOTIFY\n");

    for (;;) {
    }
}