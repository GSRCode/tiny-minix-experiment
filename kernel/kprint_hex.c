#include "prototypes.h"

void kprint_hex(unsigned int value)
{
    static const char hex[] = "0123456789ABCDEF";
    char buffer[11];
    int i;
    buffer[0] = '0'; buffer[1] = 'x'; buffer[10] = '\0';
    for (i = 0; i < 8; i++) {
        buffer[9 - i] = hex[value & 0xF];
        value >>= 4;
    }
    kprint(buffer);
}
