CC 		:= $(TARGET)-gcc --sysroot=$(SYSROOT)
AS 		:= $(TARGET)-as
LD 		:= $(TARGET)-ld
AR 		:= $(TARGET)-ar

CFLAGS 		:= \
    $(USER_CFLAGS) \
    -I. \
    -I$(SYSROOT)/include \
    -ffreestanding \
    -fno-stack-protector \
    -fno-pic -fpie \
    -mno-mmx \
    -mno-3dnow \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone

CPPFLAGS 	:=

LDFLAGS 	:= \
    -Tlinker.ld \
    -nostdlib \
    -static \
    -pie -fno-pic -fpie \
    -z max-page-size=0x1000

SOURCES 	:= $(shell find ./src -type f -name "*.c")
ASM_SOURCES 	:= $(shell find ./src -type f -name "*.S")
HEADERS 	:= $(shell find ./src -type f -name "*.h")
OBJS 		:= $(SOURCES:.c=.o) $(ASM_SOURCES:.S=.o)

.PHONY: all clean install-header install-exec

all: kernel.elf

install-headers:
	@echo "=====! INSTALLING HEADERS $(HEADERS) !====="
	mkdir -p $(SYSROOT)/include/kernel
	cp -f $(HEADERS) $(SYSROOT)/include/kernel

install-exec: kernel.elf
	@echo "=====! INSTALLING $< !====="
	mkdir -p $(SYSROOT)/boot
	cp -f kernel.elf $(SYSROOT)/boot

kernel.elf: $(OBJS)
	@echo "=====! LINKING $< INTO $@ !====="
	$(CC) $(OBJS) $(LDFLAGS) -o $@

%.o: %.c
	@echo "=====! COMPILING $< INTO $@ !====="
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.S
	@ echo =====! ASSEMBLING $< =====!
	$(AS) $(ASFLAGS) -o $@ $<

src/int/exceptions.o: src/int/exceptions.c
	@echo "=====! COMPILING $< INTO $@ !====="
	$(CC) $(CPPFLAGS) $(CFLAGS) \
	    -mgeneral-regs-only -c -o $@ $<

src/int/irqs.o: src/int/irqs.c
	@echo "=====! COMPILING $< INTO $@ !====="
	$(CC) $(CPPFLAGS) $(CFLAGS) \
	    -mgeneral-regs-only -c -o $@ $<

clean:
	rm -rf $(OBJS) kernel.elf
