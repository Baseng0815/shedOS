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

.PHONY: all clean install install-headers

all: install

install: kernel.elf
	mkdir -p $(SYSROOT)/boot
	cp -f kernel.elf $(SYSROOT)/boot

install-headers:
	mkdir -p $(SYSROOT)/include/kernel
	cp -f src/*.h $(SYSROOT)/include/kernel

kernel.elf: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(AS) $(ASFLAGS) -o $@ $<

clean:
	rm -rf $(OBJS) kernel.elf
