#ifndef GLOBALS_H
#define GLOBALS_H

#include "constants.h"
#include "descriptor.h"
#include "proc.h"
#include "message.h"

extern struct proc proc[NR_PROCS];
extern struct proc *proc_ptr;
extern struct proc *rdy_head[NR_SCHED_QUEUES];
extern struct proc *rdy_tail[NR_SCHED_QUEUES];
extern unsigned char proc_stack[NR_PROCS][K_STACK_SIZE];
extern volatile int need_resched;
extern volatile unsigned long clock_ticks;

extern volatile unsigned short *vga;
extern int cursor_row;
extern int cursor_col;

extern struct gatedesc_s idt[IDT_SIZE];
extern struct desctableptr_s idt_ptr;

#endif
