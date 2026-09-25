#include "prototypes.h"
#include "message.h"
#include "globals.h"

void proc_a(void)
{
    
    int result;
    struct message msg;

    msg.m_source = 0;
    msg.m_value = 100;


    kprint("Process A before SENDREC\n");

    __asm__ volatile (
        "movl $3, %%eax\n\t"
        "movl $1, %%ebx\n\t"
        "movl %1, %%ecx\n\t"
        "int $0x80\n\t"
        "movl %%eax, %0"
        : "=r" (result)
        : "r" (&msg)
        : "eax", "ebx", "ecx"
    );

    kprint("Process A after SENDREC\n");

    if (msg.m_value == 200)
        kprint("SENDREC SUCCESS - REPLY = 200\n");
    else
        kprint("SENDREC FAILED\n");

    for (;;) {
    }
}