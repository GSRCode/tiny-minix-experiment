# Tiny MINIX Makefile

NASM = nasm
CC = gcc
LD = ld
OBJCOPY = objcopy
QEMU = qemu-system-i386

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector
LDFLAGS = -m elf_i386 -T linker.ld


# --------------------------------------------------
# Kernel sources
# --------------------------------------------------

KERNEL_C_SOURCES = $(wildcard kernel/*.c)
KERNEL_C_OBJECTS = $(patsubst kernel/%.c,%.o,$(KERNEL_C_SOURCES))


# --------------------------------------------------
# TTY sources
# --------------------------------------------------

TTY_C_SOURCES = $(wildcard tty/*.c)
TTY_C_OBJECTS = $(patsubst tty/%.c,tty_%.o,$(TTY_C_SOURCES))


all: boot3.bin


# --------------------------------------------------
# Boot loader
# --------------------------------------------------

stage1.bin: boot/stage1.asm
	$(NASM) -f bin $< -o $@

stage2.bin: boot/stage2.asm
	$(NASM) -f bin $< -o $@


# --------------------------------------------------
# Kernel entry
# --------------------------------------------------

entry.o: kernel/entry.asm
	$(NASM) -f elf32 $< -o $@


# --------------------------------------------------
# Kernel C files
# --------------------------------------------------

%.o: kernel/%.c kernel/prototypes.h kernel/constants.h
	$(CC) $(CFLAGS) -c $< -o $@


# --------------------------------------------------
# TTY C files
# --------------------------------------------------

tty_%.o: tty/%.c kernel/prototypes.h kernel/constants.h
	$(CC) $(CFLAGS) -Ikernel -c $< -o $@


# --------------------------------------------------
# Link kernel + system processes
# --------------------------------------------------

kernel.elf: entry.o $(KERNEL_C_OBJECTS) $(TTY_C_OBJECTS) linker.ld
	$(LD) $(LDFLAGS) \
	    entry.o \
	    $(KERNEL_C_OBJECTS) \
	    $(TTY_C_OBJECTS) \
	    -o $@


# --------------------------------------------------
# Convert ELF to raw kernel binary
# --------------------------------------------------

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary $< $@
	stat -c %s $@
	@SIZE=$$(stat -c %s $@); \
	if [ $$SIZE -gt 16384 ]; then \
	    echo "ERROR: kernel.bin is $$SIZE bytes"; \
	    echo "Maximum kernel size is 16384 bytes"; \
	    rm -f $@; exit 1; \
	fi
	truncate -s 16384 $@


# --------------------------------------------------
# Final boot image
# --------------------------------------------------

boot3.bin: stage1.bin stage2.bin kernel.bin
	cat $^ > $@
	stat -c %s $@


# --------------------------------------------------
# Run
# --------------------------------------------------

run: boot3.bin
	$(QEMU) -drive format=raw,file=boot3.bin


# --------------------------------------------------
# Clean
# --------------------------------------------------

clean:
	rm -f \
	    stage1.bin \
	    stage2.bin \
	    entry.o \
	    $(KERNEL_C_OBJECTS) \
	    $(TTY_C_OBJECTS) \
	    kernel.elf \
	    kernel.bin \
	    boot3.bin