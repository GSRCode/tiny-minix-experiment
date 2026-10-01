#include "constants.h"
#include "prototypes.h"
#include "globals.h"

void keyboard_handler(void)
{
    keyboard_scan_code = inb(KEYBOARD_DATA_PORT);
    keyboard_scan_code_ready = 1;

    hardware_notify(TTY_PROC_NR);

    pic_eoi();
}