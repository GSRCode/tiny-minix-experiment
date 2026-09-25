#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Process constants. */
#define NR_PROCS 4

#define SLOT_FREE 0x01
#define SENDING 0x02
#define RECEIVING 0x04

#define P_NAME_LEN 16
#define K_STACK_SIZE 1024
#define DEFAULT_QUANTUM 50
#define SYS386_VECTOR 0x80

#define SEND 1
#define RECEIVE 2
#define SENDREC  3
#define NOTIFY   4
#define ANY (-1)

#define NORMAL_MESSAGE  0
#define NOTIFY_MESSAGE  1

#define E_DEADLOCK (-2)

/* VGA text mode constants. */
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ATTRIBUTE 0x07
#define VGA_MEMORY 0xB8000

/* 8259A PIC I/O ports and values. */
#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1
#define PIC_MASTER_OFFSET 0x20
#define PIC_SLAVE_OFFSET 0x28
#define PIC_EOI 0x20

/* PIT constants. */
#define PIT_FREQUENCY 1193182
#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43
#define PIT_COMMAND_MODE3 0x36

/* Exception vectors. */
#define DIVIDE_VECTOR 0
#define INVALID_OPCODE_VECTOR 6
#define GENERAL_PROTECTION_VECTOR 13

/* IDT / protected-mode constants. */
#define INTR_PRIVILEGE 0
#define PRESENT 0x80
#define DPL_SHIFT 5
#define INT_GATE_TYPE 0x0E
#define CS_SELECTOR 0x08
#define IDT_SIZE 256

#define NR_SCHED_QUEUES 16
#define USER_Q 7
#define IDLE_Q 15

#endif
