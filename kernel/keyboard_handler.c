#include "constants.h"
#include "prototypes.h"
#include "globals.h"

void keyboard_handler(void)
{
    unsigned char scan_code;
    int next_head;

    scan_code = inb(KEYBOARD_DATA_PORT);

    next_head = keyboard_head + 1;

    if (next_head == KEYBOARD_BUFFER_SIZE)
        next_head = 0;

    if (next_head != keyboard_tail) {
        keyboard_buffer[keyboard_head] = scan_code;
        keyboard_head = next_head;

        hardware_notify(TTY_PROC_NR);
    }

    pic_eoi();
}