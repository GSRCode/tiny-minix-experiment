#ifndef CONSOLE_H
#define CONSOLE_H

void console_clear(void);

void kprint(const char *message);

/*
 * Print a 32-bit unsigned value as:
 *
 *     0x12345678
 */
void kprint_hex(unsigned int value);

#endif