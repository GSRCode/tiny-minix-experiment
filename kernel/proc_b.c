#include "constants.h"
#include "prototypes.h"
#include "message.h"


void proc_b(void)
{
    struct message msg;
    
    kprint("B before RECEIVE\n");

    __asm__ volatile (
    "movl $2, %%eax\n\t"
    "movl $-1, %%ebx\n\t"
    "movl %0, %%ecx\n\t"
    "int $0x80"
    :
    : "r" (&msg)
    : "eax", "ebx", "ecx"
    );

    kprint("B received message\n");

    if (msg.m_type == NOTIFY_MESSAGE)
        kprint("NOTIFICATION RECEIVED - SUCCESS\n");

    for (;;) {
        }
}