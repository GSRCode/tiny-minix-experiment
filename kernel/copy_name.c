#include "constants.h"
#include "prototypes.h"

void copy_name(char *dest, const char *src)
{
    int i = 0;
    while (src[i] != '\0' && i < P_NAME_LEN - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}
