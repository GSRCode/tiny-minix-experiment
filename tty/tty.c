#include "constants.h"
#include "globals.h"
#include "message.h"
#include "prototypes.h"

void tty(void)
{
    struct message msg;
    unsigned char scan_code;

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

            while (keyboard_tail != keyboard_head) {

                scan_code = keyboard_buffer[keyboard_tail];

                keyboard_tail++;

                if (keyboard_tail == KEYBOARD_BUFFER_SIZE)
                    keyboard_tail = 0;

                kprint("TTY KEY: ");
                kprint_uint(scan_code);
                kprint(" ");
            }

        }
        
    }
}