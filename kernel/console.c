#include "console.h"


#define VGA_WIDTH  80
#define VGA_HEIGHT 25

#define VGA_ATTRIBUTE 0x07


/*
 * VGA text-mode memory starts at physical address 0xB8000.
 */
static volatile unsigned short *vga =
    (volatile unsigned short *)0xB8000;


/*
 * Software cursor position.
 */
static int cursor_row = 0;
static int cursor_col = 0;


/* * Clear all 80 x 25 VGA character cells. */
void console_clear(void)
{
    int i;

    for (i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = (VGA_ATTRIBUTE << 8) | ' '; //Attribute   = 0x07, ASCII space = 0x20
    }

    //Reset our software cursor to the top-left.
    cursor_row = 0;
    cursor_col = 0;
}


/*
 * Print a null-terminated string.
 */

void kprint(const char *message)
{
    int i = 0;

    while (message[i] != '\0')
    {
        char c = message[i];

        //Handle newline.
        if (c == '\n') {
            cursor_row++; cursor_col = 0;
        }
        else
        {
            int position = (cursor_row * VGA_WIDTH) + cursor_col;

            vga[position] = (VGA_ATTRIBUTE << 8) | (unsigned short)c ;

            cursor_col++;

            if (cursor_col >= VGA_WIDTH) { cursor_col = 0; cursor_row++; }
        }

        //Wrap back to the top if we reach the bottom, scrolling tbd
        if (cursor_row >= VGA_HEIGHT) { cursor_row = 0; }

        i++;
    }
}

/*
 * Print a 32-bit unsigned integer in hexadecimal.
 * Example: kprint_hex(0x1234);
 * prints: 0x00001234
 */
void kprint_hex(unsigned int value)
{
    static const char hex[] = "0123456789ABCDEF";

    /** 0x + 8 hexadecimal digits + '\0' requires 10 visible characters. */
    char buffer[11];

    int i;

    buffer[0] = '0'; buffer[1] = 'x'; buffer[10] = '\0';

    for (i = 0; i < 8; i++)
    {
        buffer[9 - i] = hex[value & 0xF];
        value >>= 4;
    }

    kprint(buffer);
}

void kprint_uint(unsigned long value)
{
    char buffer[11];
    char temp;
    int i,j;

    if (value == 0) { kprint("0"); return; }

    i = 0;

    while (value > 0) {
        buffer[i] = '0' + (value % 10);
        value = value / 10;
        i++;
    }

    buffer[i] = '\0';

    for (j = 0; j < i / 2; j++) {
        temp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp;
    }

    kprint(buffer);
}