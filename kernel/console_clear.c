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

void console_backspace(void)
{
    if (cursor_col == 0)
        return;

    cursor_col--;

    int position = (cursor_row * VGA_WIDTH) + cursor_col;

    vga[position] = (vga[position] & 0xFF00) | ' ';
}