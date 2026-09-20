#ifndef CONSOLE_H
#define CONSOLE_H

/*
 * Clear the VGA text screen and reset the software cursor.
 */
void console_clear(void);

/*
 * Print a null-terminated string.
 */
void kprint(const char *message);

#endif