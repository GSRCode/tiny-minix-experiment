#include "prototypes.h"
#include "message.h"
#include "globals.h"

void proc_a(void)
{
    
    struct message msg;

    msg.m_source = 0;
    msg.m_value = 1234;


    kprint("Process A before SEND\n");

    __asm__ volatile (
        "movl $1, %%eax\n\t"
        "movl $1, %%ebx\n\t"
        "movl %0, %%ecx\n\t"
        "int $0x80"
        :
        : "r" (&msg)
        : "eax", "ebx", "ecx"
    );

    kprint("Process A after SEND\n");

    for (;;) {
    }
}