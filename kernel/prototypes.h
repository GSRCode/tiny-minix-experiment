#ifndef PROTOTYPES_H
#define PROTOTYPES_H

struct desctableptr_s;
struct proc;
struct stackframe;
struct message;

/* Kernel and demonstration processes. */
void kernel_main(void);
void proc_a(void);
void proc_b(void);

/* Process management and scheduling. */
void copy_name(char *dest, const char *src);
void proc_init(void);
void proc_create(int nr, const char *name, void (*entry)(void));
void sched(void);
void sched_tick(void);
unsigned long clock_schedule(unsigned long current_sp);
void enqueue(struct proc *rp);
void dequeue(struct proc *rp);
void pick_proc(void);
int mini_send(struct proc *caller, int dst_nr, struct message *m_ptr);
int mini_receive(struct proc *caller, int src_nr, struct message *m_ptr);
int mini_sendrec(struct proc *caller, int dst_nr, struct message *m_ptr);
unsigned long sys_call(struct stackframe *frame);
void ipc_entry(void);
int deadlock(int src_nr, int dst_nr);

/* Clock. */
void clock_handler(void);

/* Console. */
void console_clear(void);
void kprint(const char *message);
void kprint_hex(unsigned int value);
void kprint_uint(unsigned long value);

/* Exceptions. */
void exception_error(unsigned int vector, unsigned int error,
                     unsigned int eip, unsigned int cs, unsigned int eflags);

/* PIC and PIT. */
void pic_init(void);
void pic_eoi(void);
void pic_unmask_irq(unsigned int irq);
void pit_init(unsigned int frequency);

/* Protected mode / IDT. */
void int_gate(unsigned int vec_nr, unsigned int offset, unsigned int dpl_type);
void prot_init(void);

/* Assembly routines. */
void divide_error(void);
void invalid_opcode(void);
void general_protection(void);
void clock_interrupt(void);
void trigger_divide_error(void);
void trigger_invalid_opcode(void);
void trigger_general_protection(void);
void enable_interrupts(void);
void cpu_halt(void);
void restore_context(unsigned long p_sp);
void delay(unsigned long secs);
unsigned long disable_interrupts(void);
void restore_flags(unsigned long flags);
void idt_load(struct desctableptr_s *ptr);
void outb(unsigned short port, unsigned char value);
unsigned char inb(unsigned short port);

#endif
