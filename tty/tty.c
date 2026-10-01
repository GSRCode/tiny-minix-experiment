#include "constants.h"
#include "globals.h"
#include "message.h"
#include "prototypes.h"

void tty(void)
{
    struct message msg;

    for (;;) {

        __asm__ volatile (
            "movl $2, %%eax\n\t"       /* RECEIVE */
            "movl %0, %%ebx\n\t"       /* HARDWARE */
            "movl %1, %%ecx\n\t"
            "int $0x80"
            :
            : "i" (HARDWARE), "r" (&msg)
            : "eax", "ebx", "ecx"
        );

        if (msg.m_source == HARDWARE &&
            msg.m_type == NOTIFY_MESSAGE) {

            if (keyboard_scan_code_ready) {

                kprint("TTY KEY: ");
                kprint_uint(keyboard_scan_code);
                kprint("\n");

                keyboard_scan_code_ready = 0;
            }
        }
    }
}