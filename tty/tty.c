#include "constants.h"
#include "globals.h"
#include "message.h"
#include "prototypes.h"

void tty(void)
{
    struct message msg;
    unsigned char scan_code;
    char ch;
    int shift_pressed = 0;
    int caps_lock = 0;

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

                //Shift pressed.
                if (scan_code == 42 || scan_code == 54) { shift_pressed = 1; continue; }

                //Shift released.
                if (scan_code == 170 || scan_code == 182) { shift_pressed = 0; continue; }

                if (scan_code == 58) { caps_lock = !caps_lock; continue; }

                if (scan_code == 14) { console_backspace(); continue; }
                        
                //Bit 7 is set for ordinary Set-1 break/release codes. Ignore releases for now.    
                if (scan_code & 0x80) continue;

                ch = scan_code_to_char(scan_code, shift_pressed, caps_lock);

                if (ch != 0) {
                    kprint_char(ch);
                }
            }

        }
        
    }
}