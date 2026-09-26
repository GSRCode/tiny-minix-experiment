#include "constants.h"
#include "prototypes.h"
#include "message.h"

void proc_a(void)
{
    struct message msg;

    msg.m_source = 0;
    msg.m_type = NORMAL_MESSAGE;
    msg.m_value = 100;

    kprint("A before SENDREC B\n");

    __asm__ volatile (
        "movl $3, %%eax\n\t"       /* SENDREC */
        "movl $1, %%ebx\n\t"       /* destination = B */
        "movl %0, %%ecx\n\t"
        "int $0x80"
        :
        : "r" (&msg)
        : "eax", "ebx", "ecx"
    );

    kprint("A SENDREC returned\n");

    /*
     * Now receive the notification that C sent while
     * A was waiting for B's reply.
     */
    kprint("A before RECEIVE ANY\n");

    __asm__ volatile (
        "movl $2, %%eax\n\t"       /* RECEIVE */
        "movl $-1, %%ebx\n\t"      /* ANY */
        "movl %0, %%ecx\n\t"
        "int $0x80"
        :
        : "r" (&msg)
        : "eax", "ebx", "ecx"
    );

    if (msg.m_type == NOTIFY_MESSAGE)
        kprint("A received pending notification - SUCCESS\n");

    for (;;) {
    }
}