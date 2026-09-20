;----------
; Stage 1 Bootloader
;
; BIOS loads this 512-byte sector at physical address 0x7C00.
; Its job is to load Stage 2 from disk into address 0x10000
; and then transfer execution to Stage 2.
;----------
bits 16                     ; Generate 16-bit x86 code because BIOS starts us in real mode.

org 0x7C00                  ; Tell NASM that this code will execute starting at address 0x7C00.


start:

    xor ax, ax              ; Set AX to zero efficiently.

    mov ds, ax              ; Set DS = 0 so data addresses refer to segment zero.


    cli                     ; Disable interrupts while changing the stack.

    mov ss, ax              ; Set stack segment SS = 0.

    mov sp, 0x7C00          ; Put stack pointer at 0x7C00; stack grows downward.

    sti                     ; Re-enable hardware interrupts now that the stack is valid.


    ; BIOS tells us which disk we booted from by placing its drive number in DL.

    mov [boot_drive], dl    ; Save the BIOS boot-drive number for later disk reads.


    ; --------------------------------------------------------
    ; Prepare memory location for Stage 2.
    ;
    ; Real-mode physical address = segment * 16 + offset.
    ;
    ; 0x1000 * 16 + 0x0000 = 0x10000.
    ; --------------------------------------------------------

    mov ax, 0x1000          ; AX = segment where Stage 2 will be loaded.

    mov es, ax              ; ES = 0x1000.

    xor bx, bx              ; BX = 0, therefore destination ES:BX = 1000:0000.


    ; --------------------------------------------------------
    ; BIOS INT 13h, function 02h: read disk sectors.
    ;
    ; AH = 02h   Read sectors
    ; AL = number of sectors
    ; CH = cylinder
    ; CL = sector number
    ; DH = head
    ; DL = drive
    ; ES:BX = destination memory address
    ; --------------------------------------------------------

    mov ah, 0x02            ; BIOS disk function 02h = read sectors.

    mov al, 1               ; Read exactly one sector containing Stage 2.

    mov ch, 0               ; Read from cylinder 0.

    mov cl, 2               ; Read sector 2; sector 1 contains this Stage 1 bootloader.

    mov dh, 0               ; Read from head 0.

    mov dl, [boot_drive]    ; Restore the BIOS drive number saved earlier.

    int 0x13                ; Ask the BIOS to read Stage 2 from disk.
    
    jc disk_error

    mov dl, [boot_drive]        ; Pass BIOS boot-drive number to Stage 2.

    jmp 0x1000:0x0000           ; Transfer execution to Stage 2.


disk_error:

    mov si, error_msg       ; SI points to the first character of our error message.


print:

    lodsb                   ; Load byte at DS:SI into AL, then increment SI.

    test al, al             ; Check whether AL is zero, marking the end of the string.

    jz hang                 ; If zero terminator was reached, stop printing.


    mov ah, 0x0E            ; BIOS INT 10h function 0Eh = teletype character output.

    int 0x10                ; Display the character currently stored in AL.

    jmp print               ; Continue with the next character.


hang:

    cli                     ; Disable maskable hardware interrupts.

    hlt                     ; Halt the CPU until an interrupt or reset occurs.

    jmp hang                ; Ensure execution remains here if the CPU resumes.


boot_drive db 0             ; Reserve one byte for the BIOS boot-drive number.

error_msg db 'Disk read error', 0
                             ; Null-terminated message displayed if disk reading fails.


times 510-($-$$) db 0       ; Pad the boot sector with zeros until byte offset 510.

dw 0xAA55                   ; Write boot signature 55 AA as the final two bytes.