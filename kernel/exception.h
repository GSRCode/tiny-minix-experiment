#ifndef EXCEPTION_H
#define EXCEPTION_H

void exception_error(
    unsigned int vector,
    unsigned int error,
    unsigned int eip,
    unsigned int cs,
    unsigned int eflags
);

#endif