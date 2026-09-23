# Tiny MINIX Makefile
NASM = nasm
CC = gcc
LD = ld
OBJCOPY = objcopy
QEMU = qemu-system-i386

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector
LDFLAGS = -m elf_i386 -T linker.ld

KERNEL_C_SOURCES = $(wildcard kernel/*.c)
KERNEL_C_OBJECTS = $(patsubst kernel/%.c,%.o,$(KERNEL_C_SOURCES))

all: boot3.bin

stage1.bin: boot/stage1.asm
	$(NASM) -f bin $< -o $@

stage2.bin: boot/stage2.asm
	$(NASM) -f bin $< -o $@

entry.o: kernel/entry.asm
	$(NASM) -f elf32 $< -o $@

%.o: kernel/%.c kernel/prototypes.h kernel/constants.h
	$(CC) $(CFLAGS) -c $< -o $@

kernel.elf: entry.o $(KERNEL_C_OBJECTS) linker.ld
	$(LD) $(LDFLAGS) entry.o $(KERNEL_C_OBJECTS) -o $@

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary $< $@
	stat -c %s $@
	@SIZE=$$(stat -c %s $@); \
	if [ $$SIZE -gt 8192 ]; then \
	    echo "ERROR: kernel.bin is $$SIZE bytes"; \
	    echo "Maximum kernel size is 8192 bytes"; \
	    rm -f $@; exit 1; \
	fi
	truncate -s 8192 $@

boot3.bin: stage1.bin stage2.bin kernel.bin
	cat $^ > $@
	stat -c %s $@

run: boot3.bin
	$(QEMU) -drive format=raw,file=boot3.bin

clean:
	rm -f stage1.bin stage2.bin entry.o $(KERNEL_C_OBJECTS) kernel.elf kernel.bin boot3.bin
