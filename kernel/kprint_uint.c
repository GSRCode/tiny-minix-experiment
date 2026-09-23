#include "prototypes.h"

void kprint_uint(unsigned long value)
{
    char buffer[11];
    char temp;
    int i, j;
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
