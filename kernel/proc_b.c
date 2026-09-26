#include "constants.h"
#include "prototypes.h"
#include "message.h"

void proc_b(void)
{
    struct message msg;

    kprint("B before RECEIVE A\n");

    __asm__ volatile (
        "movl $2, %%eax\n\t"       /* RECEIVE */
        "movl $0, %%ebx\n\t"       /* source = A */
        "movl %0, %%ecx\n\t"
        "int $0x80"
        :
        : "r" (&msg)
        : "eax", "ebx", "ecx"
    );

    kprint("B received request from A\n");

    /*
     * Give the scheduler enough time to run C.
     * We want C to notify A before B replies.
     */
    delay(1999999999);

    kprint("B sending reply to A\n");

    msg.m_source = 1;
    msg.m_type = NORMAL_MESSAGE;
    msg.m_value = 200;

    __asm__ volatile (
        "movl $1, %%eax\n\t"       /* SEND */
        "movl $0, %%ebx\n\t"       /* destination = A */
        "movl %0, %%ecx\n\t"
        "int $0x80"
        :
        : "r" (&msg)
        : "eax", "ebx", "ecx"
    );

    kprint("B reply completed\n");

    for (;;) {
    }
}