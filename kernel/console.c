/*
 * 
 * Tiny MINIX - VGA Console
 * 
 */

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


/*
 * 
 * console_clear()
 *
 * Clear all 80 x 25 VGA character cells.
 * 
 */

void console_clear(void)
{
    int i;

    /*
     * VGA has:
     *
     *     80 columns x 25 rows
     *     = 2000 character cells
     */
    for (i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
    {
        /*
         * ASCII space = 0x20
         * Attribute   = 0x07
         *
         * Result:
         *
         *     0x0720
         *
         * This displays a blank character.
         */
        vga[i] =
            (VGA_ATTRIBUTE << 8) | ' ';
    }


    /*
     * Reset our software cursor to the top-left.
     */
    cursor_row = 0;
    cursor_col = 0;
}


/*
 * 
 * kprint()
 *
 * Print a null-terminated string.
 * 
 */

void kprint(const char *message)
{
    int i = 0;

    while (message[i] != '\0')
    {
        char c = message[i];


        /*
         * Handle newline.
         */
        if (c == '\n')
        {
            cursor_row++;
            cursor_col = 0;
        }
        else
        {
            /*
             * Convert row/column into VGA cell number.
             */
            int position =
                (cursor_row * VGA_WIDTH) + cursor_col;


            /*
             * Store character and VGA attribute.
             */
            vga[position] =
                (unsigned short)c |
                (VGA_ATTRIBUTE << 8);


            cursor_col++;


            /*
             * Automatic line wrapping.
             */
            if (cursor_col >= VGA_WIDTH)
            {
                cursor_col = 0;
                cursor_row++;
            }
        }


        /*
         * Temporary behaviour:
         *
         * Wrap back to the top if we reach the bottom.
         * Later we can implement scrolling.
         */
        if (cursor_row >= VGA_HEIGHT)
        {
            cursor_row = 0;
        }


        i++;
    }
}