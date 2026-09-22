#ifndef STACKFRAME_H
#define STACKFRAME_H

/** Saved processor state for a process.*/
struct stackframe {
    unsigned long gs; unsigned long fs; unsigned long es; unsigned long ds;
    unsigned long edi; unsigned long esi; unsigned long ebp; unsigned long esp;
    unsigned long ebx; unsigned long edx; unsigned long ecx; unsigned long eax;
    unsigned long eip; unsigned long cs; unsigned long eflags;
};

#endif