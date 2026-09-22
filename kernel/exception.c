#include "console.h"
#include "exception.h"
#include "protect.h"

void exception_error( unsigned int vector, unsigned int error, unsigned int eip, unsigned int cs, unsigned int eflags)
{
    kprint("\nCPU EXCEPTION\n");

    if (vector == GENERAL_PROTECTION_VECTOR) { kprint("Vector 13: General Protection Fault\n"); }

    kprint("ERROR  = "); kprint_hex(error); kprint("\n");
    kprint("EIP    = "); kprint_hex(eip); kprint("\n");
    kprint("CS     = "); kprint_hex(cs); kprint("\n");
    kprint("EFLAGS = "); kprint_hex(eflags); kprint("\n");

    kprint("System halted.\n");
}