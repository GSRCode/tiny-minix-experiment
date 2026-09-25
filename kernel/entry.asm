; 
; Tiny MINIX Kernel - Assembly Entry
;
; Responsibilities:
;
;   1. Start in 16-bit real mode
;   2. Load the GDT
;   3. Enable protected mode
;   4. Enter 32-bit mode
;   5. Initialize segment registers
;   6. Create a kernel stack
;   7. Call kernel_main() written in C
;
; Stage 2 loads this kernel at physical address 0x20000.
; 


BITS 16

SECTION .text

GLOBAL _start
GLOBAL idt_load

GLOBAL divide_error
GLOBAL invalid_opcode
GLOBAL general_protection
GLOBAL clock_interrupt

GLOBAL trigger_divide_error
GLOBAL trigger_invalid_opcode
GLOBAL trigger_general_protection

GLOBAL outb
GLOBAL inb
GLOBAL enable_interrupts
GLOBAL cpu_halt
GLOBAL restore_context
GLOBAL delay
GLOBAL disable_interrupts
GLOBAL restore_flags
global ipc_entry


EXTERN kernel_main              ; kernel_main() is defined in main.c
EXTERN exception
EXTERN exception_error
EXTERN clock_handler
EXTERN clock_schedule
extern sys_call


EXTERN __bss_start
EXTERN __bss_end
EXTERN proc_ptr

; 
; 16-bit real-mode entry
; 

_start:

    cli                         ; Disable interrupts during mode switch.


    ; --------------------------------------------------------
    ; Stage 2 enters us at:
    ;
    ;     CS = 0x2000
    ;     IP = 0x0000
    ;
    ; Set DS to the same segment.
    ; --------------------------------------------------------

    mov ax, cs

    mov ds, ax


    ; --------------------------------------------------------
    ; Load the Global Descriptor Table.
    ;
    ; DS already contributes physical base 0x20000.
    ;
    ; Therefore we use the offset of gdt_descriptor relative
    ; to the beginning of the kernel.
    ; --------------------------------------------------------

    lgdt [gdt_descriptor - _start]


    ; --------------------------------------------------------
    ; Enable protected mode.
    ;
    ; CR0 bit 0 = PE (Protected Mode Enable).
    ; --------------------------------------------------------

    mov eax, cr0

    or eax, 0x00000001

    mov cr0, eax


    ; --------------------------------------------------------
    ; Far jump:
    ;
    ;     CS  = CODE_SELECTOR = 0x08
    ;     EIP = protected_mode
    ;
    ; This also starts execution using our 32-bit code
    ; descriptor.
    ; --------------------------------------------------------

    jmp dword CODE_SELECTOR:protected_mode



; 
; Global Descriptor Table
; 

ALIGN 8


gdt_start:


; ------------------------------------------------------------
; Descriptor 0: Null descriptor
;
; Selector = 0x00
; ------------------------------------------------------------

gdt_null:

    dq 0x0000000000000000



; ------------------------------------------------------------
; Descriptor 1: 32-bit flat CODE segment
;
; Base  = 0x00000000
; Limit = 0xFFFFFFFF
;
; Selector = 0x08
; ------------------------------------------------------------

gdt_code:

    dw 0xFFFF                   ; Limit bits 0-15

    dw 0x0000                   ; Base bits 0-15

    db 0x00                     ; Base bits 16-23

    db 10011010b                ; Present, ring 0, code, readable

    db 11001111b                ; 4 KB granularity, 32-bit segment

    db 0x00                     ; Base bits 24-31



; ------------------------------------------------------------
; Descriptor 2: 32-bit flat DATA segment
;
; Base  = 0x00000000
; Limit = 0xFFFFFFFF
;
; Selector = 0x10
; ------------------------------------------------------------

gdt_data:

    dw 0xFFFF                   ; Limit bits 0-15

    dw 0x0000                   ; Base bits 0-15

    db 0x00                     ; Base bits 16-23

    db 10010010b                ; Present, ring 0, writable data

    db 11001111b                ; 4 KB granularity, 32-bit segment

    db 0x00                     ; Base bits 24-31



gdt_end:



; 
; GDTR structure
; 

gdt_descriptor:

    dw gdt_end - gdt_start - 1  ; GDT limit

    dd gdt_start                ; Linear address of GDT



; 
; GDT selectors
; 

CODE_SELECTOR equ 0x08

DATA_SELECTOR equ 0x10



; 
; 32-bit protected-mode code
; 

BITS 32


protected_mode:


    ; --------------------------------------------------------
    ; CS was loaded with 0x08 by the far jump.
    ;
    ; Load our data selector into all data segment registers.
    ; --------------------------------------------------------

    mov ax, DATA_SELECTOR

    mov ds, ax

    mov es, ax

    mov fs, ax

    mov gs, ax

    mov ss, ax


    ; --------------------------------------------------------
    ; Establish temporary kernel stack.
    ;
    ; Stack grows downward from address 0x90000.
    ; --------------------------------------------------------

    mov esp, 0x90000

    ; ---------------------------------------------------------
    ; Clear the .bss section.
    ;
    ; C requires uninitialized global/static variables to
    ; initially contain zero.
    ; ---------------------------------------------------------

    ; EDI points to the beginning of .bss.
    mov edi, __bss_start

    ; ECX will contain the number of bytes in .bss.
    mov ecx, __bss_end

    ; Calculate:
    ;
    ; ECX = __bss_end - __bss_start
    ;
    ; This gives us the size of .bss in bytes.
    sub ecx, edi

    ; AL = 0.
    xor eax, eax

    ; Make string instructions move forward through memory.
    cld

    ; Write AL (zero) to ES:EDI, ECX times.
    ;
    ; Since ES uses our flat data segment, EDI is effectively
    ; the linear address of .bss.
    rep stosb


    ; --------------------------------------------------------
    ; Call our C kernel.
    ;
    ; The linker resolves kernel_main to the address of the
    ; function contained in main.o.
    ;
    ; CALL also pushes the return address onto our new stack.
    ; --------------------------------------------------------

    call kernel_main


    ; --------------------------------------------------------
    ; kernel_main() should never return.
    ;
    ; But if it does, halt safely here.
    ; --------------------------------------------------------

kernel_halt:

    cli

    hlt

    jmp kernel_halt



divide_error:
    push dword 0          ; dummy error code
    push dword 0               ; #DE = vector 0
    jmp common_error_exception


invalid_opcode:
    push dword 0          ; dummy error code
    push dword 6               ; #UD = vector 6
    jmp common_error_exception

general_protection:

    ; Unlike #DE and #UD, the CPU has already pushed
    ; an error code for #GP.
    ;
    ; On entry:
    ;
    ;   [ESP + 0]  = error code
    ;   [ESP + 4]  = EIP
    ;   [ESP + 8]  = CS
    ;   [ESP + 12] = EFLAGS
    ;
    ; Add the vector number above the CPU error code.

    push dword 13

    jmp common_error_exception

common_error_exception:
    cli

    ; Stack:
    ;
    ; [ESP + 16] = EFLAGS
    ; [ESP + 12] = CS
    ; [ESP + 8]  = EIP
    ; [ESP + 4]  = CPU error code
    ; [ESP + 0]  = vector
    ;
    ; C function:
    ;
    ; exception_error(vector, error, eip, cs, eflags)
    ;
    ; cdecl pushes arguments right-to-left.

    push dword [esp + 16]      ; EFLAGS
    push dword [esp + 16]      ; CS
    push dword [esp + 16]      ; EIP
    push dword [esp + 16]      ; error code
    push dword [esp + 16]      ; vector

    call exception_error

    ; Five arguments x 4 bytes.
    add esp, 20

common_error_exception_halt:
    hlt
    jmp common_error_exception_halt

; ============================================================
; Hardware interrupt: IRQ0 - System timer
;
; The master 8259A PIC has been remapped so:
;
;     IRQ0 -> IDT vector 0x20
;
; For now this is only a placeholder.
;
; IRQ0 is still masked at the PIC and interrupts are still
; disabled globally, so this handler should not execute yet.
; ============================================================

clock_interrupt:

    ; CPU has already pushed:
    ;     EFLAGS, CS, EIP

    pushad


    ;Preserve the address of that frame.
    mov ebx, esp

    ;Run the normal C clock handler.

    call clock_handler

    ; clock_schedule(current_sp) - ask scheduler which process's context to be restored

    push ebx ; parameter to clock_schedule
    call clock_schedule
    add esp, 4

    ;EAX now contains the saved stack pointer of the process that should run - same or different

    mov esp, eax


    popad
    iretd

; ---------------------------------------------------------
; Load Interrupt Descriptor Table Register (IDTR).
;
; C calls:
;
;     idt_load(&idt_ptr);
;
; With the 32-bit GCC cdecl calling convention, the first
; argument is located at [ESP + 4].
; ---------------------------------------------------------

idt_load:
    ; Get the address of the IDTR structure passed by C.
    mov eax, [esp + 4]

    ; Load IDTR from the 6-byte structure.
    lidt [eax]

    ; Return to the C caller.
    ret

; ---------------------------------------------------------
; Deliberately trigger CPU exception 0: Divide Error (#DE).
;
; This function exists only for testing our IDT.
; ---------------------------------------------------------

trigger_divide_error:

    ; Dividend = 1.
    mov eax, 1

    ; EDX:EAX is the dividend for a 32-bit DIV.
    ; Clear the high 32 bits.
    xor edx, edx

    ; Divisor = 0.
    xor ecx, ecx

    ; Attempt:
    ;
    ;     EDX:EAX / ECX
    ;
    ; Since ECX is zero, the CPU generates #DE,
    ; exception vector 0.
    div ecx

    ; We should never reach here.
    ret

trigger_invalid_opcode:

    ; UD2 is specifically provided by x86 for generating
    ; an Invalid Opcode exception (#UD).
    ;
    ; CPU should invoke vector 6.

    ud2

    ; We should never reach this RET because our
    ; exception handler halts the machine.

    ret

trigger_general_protection:

    ; 0x18 would refer to GDT entry 3.
    ;
    ; Our tiny GDT only contains entries 0, 1 and 2,
    ; so this selector is outside our GDT limit.
    ;
    ; Loading it into DS should generate #GP.

    mov ax, 0x18
    mov ds, ax

    ; We should never reach here.

    ret

; ============================================================
; void outb(unsigned short port, unsigned char value);
;
; 32-bit cdecl stack on entry:
;
;   [ESP + 4] = port
;   [ESP + 8] = value
;
; OUT DX, AL writes one byte to an x86 I/O port.
; ============================================================

outb:
    mov dx, [esp + 4]       ; DX = port
    mov al, [esp + 8]       ; AL = value

    out dx, al

    ret

; ============================================================
; Read one byte from an I/O port.
;
; unsigned char inb(unsigned short port);
;
; C calling convention:
;     [esp + 4] = port
;
; Return value:
;     AL = byte read from port
; ============================================================

inb:
    mov edx, [esp + 4]
    in al, dx
    ret

; ============================================================
; Enable maskable hardware interrupts.
;
; void enable_interrupts(void);
;
; STI sets the Interrupt Flag (IF) in EFLAGS.
; After this, the CPU can accept maskable hardware interrupts
; such as IRQ0 from the timer.
; ============================================================

enable_interrupts:
    sti
    ret

; ============================================================
; Halt the CPU until the next hardware interrupt.
;
; void cpu_halt(void);
;
; With interrupts enabled, HLT stops instruction execution
; until an interrupt arrives.
; ============================================================

cpu_halt:
    hlt
    ret




; void delay(unsigned int count);
;
; count is passed on the stack:
; [esp + 4] = count

delay:
    push ebp
    mov  ebp, esp

    mov  ecx, [ebp + 8]

.loop:
    nop
    loop .loop

    pop  ebp
    ret

restore_context:

    mov eax, [esp + 4]

    ; Switch to the selected process's saved frame.
    mov esp, eax

    ; Restore general-purpose registers.
    popad

    ; Restore:
    ;
    ; EIP
    ; CS
    ; EFLAGS
    ;
    ; and resume the process.
    iretd


disable_interrupts:

    pushfd
    pop eax

    cli

    ret

restore_flags:

    mov eax, [esp + 4]

    push eax
    popfd

    ret

ipc_entry:
    pushad

    ; ESP now points to the saved stackframe.
    push esp
    call sys_call
    add esp, 4

    ; sys_call returns the selected process's
    ; saved stack pointer in EAX.
    mov esp, eax

    popad
    iretd