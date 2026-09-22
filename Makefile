# ============================================================
# Tiny MINIX Makefile
# ============================================================


# ------------------------------------------------------------
# Tools
# ------------------------------------------------------------

NASM    = nasm
CC      = gcc
LD      = ld
OBJCOPY = objcopy
QEMU    = qemu-system-i386


# ------------------------------------------------------------
# Compiler / assembler flags
# ------------------------------------------------------------

CFLAGS = -m32 \
         -ffreestanding \
         -fno-pie \
         -fno-stack-protector

LDFLAGS = -m elf_i386 -T linker.ld


# ------------------------------------------------------------
# Default target
# ------------------------------------------------------------

all: boot3.bin


# ------------------------------------------------------------
# Stage 1
# ------------------------------------------------------------

stage1.bin: boot/stage1.asm
	$(NASM) -f bin boot/stage1.asm -o stage1.bin


# ------------------------------------------------------------
# Stage 2
# ------------------------------------------------------------

stage2.bin: boot/stage2.asm
	$(NASM) -f bin boot/stage2.asm -o stage2.bin


# ------------------------------------------------------------
# Kernel assembly entry
# ------------------------------------------------------------

entry.o: kernel/entry.asm
	$(NASM) -f elf32 kernel/entry.asm -o entry.o


# ------------------------------------------------------------
# Kernel C files
# ------------------------------------------------------------

main.o: kernel/main.c kernel/console.h
	$(CC) $(CFLAGS) -c kernel/main.c -o main.o


console.o: kernel/console.c kernel/console.h
	$(CC) $(CFLAGS) -c kernel/console.c -o console.o


protect.o: kernel/protect.c kernel/protect.h kernel/descriptor.h
	$(CC) $(CFLAGS) -c kernel/protect.c -o protect.o

exception.o: kernel/exception.c kernel/exception.h kernel/console.h
	$(CC) $(CFLAGS) -c kernel/exception.c -o exception.o

pic.o: kernel/pic.c kernel/pic.h kernel/io.h
	$(CC) $(CFLAGS) -c kernel/pic.c -o pic.o

clock.o: kernel/clock.c kernel/clock.h
	$(CC) $(CFLAGS) -c kernel/clock.c -o clock.o

pit.o: kernel/pit.c kernel/pit.h
	$(CC) $(CFLAGS) -c kernel/pit.c -o pit.o

# ------------------------------------------------------------
# Link kernel ELF
# ------------------------------------------------------------

kernel.elf: entry.o main.o console.o protect.o exception.o pic.o clock.o pit.o linker.ld
	$(LD) $(LDFLAGS) \
	    entry.o main.o console.o protect.o exception.o pic.o clock.o pit.o \
	    -o kernel.elf

# ------------------------------------------------------------
# Convert ELF kernel to raw binary
#
# Our Stage 2 currently reads exactly one 512-byte sector.
# Therefore pad kernel.bin to exactly 512 bytes.
# ------------------------------------------------------------

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.bin
	stat -c %s kernel.bin
	@SIZE=$$(stat -c %s kernel.bin); \
	if [ $$SIZE -gt 8192 ]; then \
	    echo "ERROR: kernel.bin is $$SIZE bytes"; \
	    echo "Maximum kernel size is 8192 bytes"; \
	    rm -f kernel.bin; \
	    exit 1; \
	fi
	truncate -s 8192 kernel.bin

# ------------------------------------------------------------
# Construct bootable disk image
#
# Sector 1 = Stage 1
# Sector 2 = Stage 2
# Sector 3 = Kernel
# ------------------------------------------------------------

boot3.bin: stage1.bin stage2.bin kernel.bin
	cat stage1.bin stage2.bin kernel.bin > boot3.bin
	stat -c %s boot3.bin


# ------------------------------------------------------------
# Run Tiny MINIX in QEMU
# ------------------------------------------------------------

run: boot3.bin
	$(QEMU) -drive format=raw,file=boot3.bin


# ------------------------------------------------------------
# Remove generated files
# ------------------------------------------------------------

clean:
	rm -f \
	    stage1.bin \
	    stage2.bin \
	    entry.o \
	    main.o \
	    console.o \
		exception.o \
		pic.o \
		clock.o \
		pit.o \
		protect.o \
	    kernel.elf \
	    kernel.bin \
	    boot3.bin