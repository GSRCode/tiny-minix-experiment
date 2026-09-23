#include "constants.h"
#include "globals.h"
#include "prototypes.h"

void kprint(const char *message)
{
    int i = 0;
    while (message[i] != '\0') {
        char c = message[i];
        if (c == '\n') {
            cursor_row++;
            cursor_col = 0;
        } else {
            int position = (cursor_row * VGA_WIDTH) + cursor_col;
            vga[position] = (VGA_ATTRIBUTE << 8) | (unsigned short)c;
            cursor_col++;
            if (cursor_col >= VGA_WIDTH) {
                cursor_col = 0;
                cursor_row++;
            }
        }
        if (cursor_row >= VGA_HEIGHT) cursor_row = 0;
        i++;
    }
}
