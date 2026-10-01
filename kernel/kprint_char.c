#include "prototypes.h"

void kprint_char(char c)
{
    char str[2];

    str[0] = c;
    str[1] = '\0';

    kprint(str);
}