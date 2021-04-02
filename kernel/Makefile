CC 	:= $(TARGET)-gcc --sysroot=$(SYSROOT)
AS 	:= $(TARGET)-as
LD 	:= $(TARGET)-ld
AR 	:= $(TARGET)-ar

CFLAGS 	:= $(USER_CFLAGS) \
    -I. \
    -ffreestanding \
    -fno-stack-protector \
    -fno-pic -fpie \
    -mno-red-zone

LDFLAGS := \
    -Tlinker.ld \
    -nostdlib \
    -shared \
    -pie -fno-pic -fpie \
    -z max-page-size=0x1000

SOURCES := $(shell find ./src -type f -name "*.c")
HEADERS := $(shell find ./src -type f -name "*.h")
OBJS 	:= $(SOURCES:.c=.o)

.PHONY: all clean install-header install-exec

all: kernel.elf

install-headers:
	mkdir -p $(SYSROOT)/include/kernel
	cp $(HEADERS) $(SYSROOT)/include/kernel

install-exec: kernel.elf
	mkdir -p $(SYSROOT)/boot
	cp kernel.elf $(SYSROOT)/boot

kernel.elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

%o: %c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) kernel.elf
