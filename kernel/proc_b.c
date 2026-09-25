#include "prototypes.h"
#include "message.h"

void proc_b(void)
{
    struct message msg;

    kprint("Process B before RECEIVE\n");

    __asm__ volatile (
        "movl $2, %%eax\n\t"
        "movl $-1, %%ebx\n\t"
        "movl %0, %%ecx\n\t"
        "int $0x80"
        :
        : "r" (&msg)
        : "eax", "ebx", "ecx"
    );

    kprint("Process B received message\n");

    if (msg.m_value == 1234)
        kprint("MESSAGE VALUE = 1234 - SUCCESS\n");
    else
        kprint("MESSAGE VALUE INCORRECT\n");

    for (;;) {
    }
}