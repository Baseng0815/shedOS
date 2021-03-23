EXTERNAL?=

CC?=gcc
LD?=ld
AR?=ar
LDS=kernel.ld

override CFLAGS+=-ffreestanding -fno-stack-protector -fshort-wchar \
    -I/home/bastian/shedOS/sysroot/include \
    -mno-red-zone

LDFLAGS=-T$(LDS) \
	-static \
	-Bsymbolic \
	-nostdlib \
	-L$(SYSROOT)/lib \
	-lk

ASFLAGS=

OBJS:=src/main.o src/terminal.o src/framebuffer.o src/font.o \
    src/cpuinfo.o src/memory/efi_memory.o src/memory/pfa.o \
    src/memory/bitmap.o src/memory/paging.o src/gdt/gdt.o \
    src/int/interrupts.o src/int/idt.o

.PHONY: all clean install install-headers

all: install

install: kernel.elf
	@ echo =====! INSTALLING kernel.elf =====!
	mkdir -p $(SYSROOT)/boot
	cp -f kernel.elf $(SYSROOT)/boot

install-headers:
	@ echo =====! INSTALLING HEADERS =====!
	mkdir -p $(SYSROOT)/include/kernel
	cp -f src/*.h $(SYSROOT)/include/kernel

kernel.elf: $(OBJS)
	@ echo =====! LINKING $^ =====!
	$(LD) $(OBJS) $(LDFLAGS) -o kernel.elf

%.o: %.c
	@ echo =====! COMPILING $< =====!
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	@ echo =====! ASSEMBLING $< =====!
	$(AS) $(ASFLAGS) -o $@ $<

src/int/interrupts.o: src/int/interrupts.c
	@ echo =====! COMPILING $< =====!
	$(CC) $(CFLAGS) -mgeneral-regs-only -c -o $@ $<

clean:
	rm -rf $(OBJS) kernel.elf
