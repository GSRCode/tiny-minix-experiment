#include "constants.h"
#include "prototypes.h"
#include "message.h"


void proc_b(void)
{
    int result; 
    struct message msg; msg.m_source = 1; msg.m_value = 5678;

    kprint("B before trying to send to A \n");

    __asm__ volatile (
        "movl $1, %%eax\n\t"
        "movl $0, %%ebx\n\t"
        "movl %1, %%ecx\n\t"
        "int $0x80\n\t"
        "movl %%eax, %0"
        : "=r" (result)
        : "r" (&msg)
        : "eax", "ebx", "ecx"
    );

    if (result == E_DEADLOCK)
        kprint("DEADLOCK DETECTED - SUCCESS\n");
    else
        kprint("DEADLOCK NOT DETECTED\n");    
    for (;;) {
        }
}