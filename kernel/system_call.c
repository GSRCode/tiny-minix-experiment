#include "constants.h"
#include "globals.h"
#include "prototypes.h"

unsigned long system_call(int call_nr)
{
    switch (call_nr) {

    case SYS_GETUPTIME:
        return get_uptime_ticks();

    default:
        return 0;
    }
}