#include "constants.h"
#include "prototypes.h"

void keyboard_handler(void)
{
    unsigned char scan_code;

    scan_code = inb(KEYBOARD_DATA_PORT);

    kprint("KEY: ");
    kprint_uint(scan_code);
    kprint("\n");

    pic_eoi();
}