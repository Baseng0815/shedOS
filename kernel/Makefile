EXTERNAL?=

CC=clang
LD=ld.lld
LDS=kernel.ld

CFLAGS=-ffreestanding -fshort-wchar
LDFLAGS=-T$(LDS) \
	-static \
	-Bsymbolic \
	-nostdlib

OBJS:=src/main.o

.PHONY: all clean install

all: install

install: kernel.elf
	mkdir -p $(DESTDIR)
	cp -f kernel.elf $(DESTDIR)

kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) -std=gnu11 -c -o $@ $<

clean:
	rm -rf $(OBJS)
