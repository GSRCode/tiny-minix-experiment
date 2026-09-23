#include "constants.h"
#include "globals.h"
#include "prototypes.h"

void console_clear(void)
{
    int i;
    for (i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = (VGA_ATTRIBUTE << 8) | ' ';
    }
    cursor_row = 0;
    cursor_col = 0;
}
