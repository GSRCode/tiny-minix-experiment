#include "constants.h"
#include "prototypes.h"
#include "message.h"


void proc_b(void)
{
    struct message msg;

    kprint("B before RECEIVE\n");

    __asm__ volatile (
        "movl $2, %%eax\n\t"
        "movl $0, %%ebx\n\t"
        "movl %0, %%ecx\n\t"
        "int $0x80"
        :
        : "r" (&msg)
        : "eax", "ebx", "ecx"
    );

    kprint("B received request\n");

    if (msg.m_value == 100)
        kprint("REQUEST VALUE = 100 - SUCCESS\n");

    msg.m_value = 200;

    kprint("B sending reply\n");

    __asm__ volatile (
    "movl $1, %%eax\n\t"
    "movl $0, %%ebx\n\t"
    "movl %0, %%ecx\n\t"
    "int $0x80"
    :
    : "r" (&msg)
    : "eax", "ebx", "ecx"
    );

    for (;;) {
        }
}