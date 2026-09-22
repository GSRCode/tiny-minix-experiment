#ifndef IO_H
#define IO_H

/*
 * port  = 16-bit I/O port number
 * value = 8-bit value to write
 */
void outb(unsigned short port, unsigned char value);

unsigned char inb(unsigned short port);

#endif