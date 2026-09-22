#ifndef CLOCK_H
#define CLOCK_H

extern volatile unsigned long clock_ticks;

void clock_handler(void);

#endif