EXTERNAL?=

CC?=clang
LD?=lld-link
LDS=kernel.ld

CFLAGS=-ffreestanding -fno-stack-protector \
       -Iinclude

LDFLAGS=-T$(LDS) \
	-static \
	-nostdlib \
	-L$(SYSROOT)/lib \
	-lk

OBJS:=src/main.o src/terminal.o src/framebuffer.o src/font.o

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
	$(CC) $(CFLAGS) -std=gnu11 -c -o $@ $<

clean:
	rm -rf $(OBJS) kernel.elf
