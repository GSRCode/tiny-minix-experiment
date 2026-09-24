#include "globals.h"

struct proc proc[NR_PROCS];
struct proc *proc_ptr;
struct proc *rdy_head[NR_SCHED_QUEUES];
struct proc *rdy_tail[NR_SCHED_QUEUES];
unsigned char proc_stack[NR_PROCS][K_STACK_SIZE];
volatile int need_resched = 0;
volatile unsigned long clock_ticks = 0;

volatile unsigned short *vga = (volatile unsigned short *)VGA_MEMORY;
int cursor_row = 0;
int cursor_col = 0;

struct gatedesc_s idt[IDT_SIZE];
struct desctableptr_s idt_ptr;
