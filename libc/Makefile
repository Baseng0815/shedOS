EXTERNAL?=

CC?=clang
LD?=ld

CFLAGS=-ffreestanding -fno-stack-protector \
       -Iinclude \
       -static
LDFLAGS=

LIBK_CFLAGS=$(CFLAGS) -D__IS_LIBK

OBJS:=src/string.o src/printk.o

.PHONY: all clean install install-headers

install: libk.a
	mkdir -p $(SYSROOT)/lib
	cp -f libk.a $(SYSROOT)/lib

install-headers:
	mkdir -p $(SYSROOT)/include
	cp -f src/*.h $(SYSROOT)/include

libk.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

# TODO add .libk.o suffix to distinguish libc and libk (can be done later on)
%.o: %.c
	$(CC) $(LIBK_CFLAGS) -std=gnu11 -c $< -o $@

clean:
	rm -rf $(OBJS) libk.a
