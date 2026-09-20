;----------
; Stage 2 - Tiny MINIX Boot Monitor
;
; Stage 1 loads this program from disk sector 2 into:
;
;     1000:0000
;
; Physical address:
;
;     0x1000 * 16 = 0x10000
;
; Stage 2 will:
;
;   1. Print "Tiny MINIX Boot Monitor"
;   2. Load kernel.bin from disk sector 3
;   3. Place kernel at physical address 0x20000
;   4. Jump to the kernel at 2000:0000
;----------
;

bits 16                     ; We are still running in 16-bit real mode.

org 0x0000                  ; Stage 2 starts at offset zero in segment 0x1000.


start:

    mov ax, cs              ; AX = current code segment, which is 0x1000.

    mov ds, ax              ; DS = CS so we can access Stage 2 variables and strings.

    mov [boot_drive], dl    ; Save the BIOS boot-drive number passed by Stage 1.


    ; --------------------------------------------------------
    ; Print Stage 2 startup message.
    ; --------------------------------------------------------

    mov si, message         ; SI points to our null-terminated message.


print:

    lodsb                   ; Load byte at DS:SI into AL and increment SI.

    test al, al             ; Check whether AL is zero.

    jz load_kernel          ; Zero means end of string, so start loading kernel.

    mov ah, 0x0E            ; BIOS INT 10h function 0Eh = teletype output.

    mov bh, 0               ; Select display page zero.

    int 0x10                ; Print the character contained in AL.

    jmp print               ; Continue with the next character.


load_kernel:

    ; --------------------------------------------------------
    ; Load kernel.bin.
    ;
    ; Disk layout:
    ;
    ; Sector 1 = Stage 1
    ; Sector 2 = Stage 2
    ; Sector 3 = kernel.bin
    ;
    ; We load the kernel into:
    ;
    ;     ES:BX = 2000:0000
    ;
    ; Physical address:
    ;
    ;     0x2000 * 16 + 0 = 0x20000
    ; --------------------------------------------------------

    mov ax, 0x2000          ; AX = kernel destination segment.

    mov es, ax              ; ES = 0x2000.

    xor bx, bx              ; BX = 0, giving destination 2000:0000.


    ; --------------------------------------------------------
    ; BIOS INT 13h, function 02h = read disk sectors.
    ; --------------------------------------------------------

    mov ah, 0x02            ; BIOS disk read function.

    mov al, 16                 ; Read 16 sectors = 8 KiB

    mov ch, 0               ; Cylinder zero.

    mov cl, 3               ; Sector three contains kernel.bin.

    mov dh, 0               ; Head zero.

    mov dl, [boot_drive]    ; Use the same disk from which we booted.

    int 0x13                ; BIOS loads kernel sector into ES:BX.


    jc disk_error           ; Carry Flag means BIOS reported a disk-read error.


    ; --------------------------------------------------------
    ; Kernel has now been loaded successfully.
    ;
    ; Transfer execution to:
    ;
    ;     CS:IP = 2000:0000
    ;
    ; This corresponds to physical address 0x20000.
    ; --------------------------------------------------------

    jmp 0x2000:0x0000       ; Far jump from Stage 2 into our kernel.


disk_error:

    ; --------------------------------------------------------
    ; Display an error if BIOS could not read kernel.bin.
    ; --------------------------------------------------------

    mov si, error_message   ; SI points to disk-error message.


print_error:

    lodsb                   ; Load next error-message character into AL.

    test al, al             ; Check for null terminator.

    jz hang                 ; End of message: stop execution.

    mov ah, 0x0E            ; BIOS teletype output function.

    mov bh, 0               ; Display page zero.

    int 0x10                ; Print character.

    jmp print_error         ; Print next character.


hang:

    cli                     ; Disable maskable hardware interrupts.

    hlt                     ; Halt the processor.

    jmp hang                ; Remain here if processor resumes.


boot_drive db 0             ; Storage for BIOS boot-drive number.

message db 'Tiny MINIX Boot Monitor', 13, 10, 0
                             ; Stage 2 startup message.

error_message db 'Kernel disk read error', 13, 10, 0
                             ; Message displayed if kernel loading fails.


times 512-($-$$) db 0       ; Pad Stage 2 to exactly one 512-byte sector.