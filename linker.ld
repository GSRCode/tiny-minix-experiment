/* 
 * Tiny MINIX Kernel Linker Script
 *
 * Stage 2 loads kernel.bin at physical address 0x20000.
 *
 * Therefore the linker arranges the kernel as though its
 * linear address begins at 0x20000.
 * */

ENTRY(_start)

SECTIONS
{
    . = 0x20000;

    .text :
    {
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        __bss_start = .;

        *(COMMON)
        *(.bss)

        __bss_end = .;
    }
}