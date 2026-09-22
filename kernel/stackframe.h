#ifndef STACKFRAME_H
#define STACKFRAME_H

/** Saved processor state for a process.*/
struct stackframe {
    unsigned long edi; unsigned long esi; unsigned long ebp; unsigned long esp_dummy;
    unsigned long ebx; unsigned long edx; unsigned long ecx; unsigned long eax;
    unsigned long eip; unsigned long cs; unsigned long eflags;
};
#endif