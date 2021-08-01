CC 		:= $(TARGET)-gcc --sysroot=$(SYSROOT)
AS 		:= $(TARGET)-as
LD 		:= $(TARGET)-ld
AR 		:= $(TARGET)-ar

# 1920x1080: FB_FHD
# 2560x1440: FB_QHD
FB_SIZE 	:= FB_FHD

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
    -mno-red-zone \
    -D$(FB_SIZE)

CPPFLAGS 	:=

LDFLAGS 	:= \
    -Tlinker.ld \
    -nostdlib \
    -static \
    -pie -fno-pic -fpie \
    -z max-page-size=0x1000

ASFLAGS 	:=

SOURCES 	:= $(shell find ./src -type f -name "*.c")
ASM_SOURCES 	:= $(shell find ./src -type f -name "*.S")
HEADERS 	:= $(shell find ./src -type f -name "*.h")
OBJS 		:= $(SOURCES:.c=_c.o) $(ASM_SOURCES:.S=_S.o)

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

%_c.o: %.c
	@echo "=====! COMPILING $< INTO $@ !====="
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

%_S.o: %.S
	@ echo =====! ASSEMBLING $< =====!
	$(AS) $(ASFLAGS) -o $@ $<

src/interrupts/exceptions_c.o: src/interrupts/exceptions.c
	@echo "=====! COMPILING $< INTO $@ !====="
	$(CC) $(CPPFLAGS) $(CFLAGS) \
	    -mgeneral-regs-only -c -o $@ $<

src/interrupts/irqs_c.o: src/interrupts/irqs.c
	@echo "=====! COMPILING $< INTO $@ !====="
	$(CC) $(CPPFLAGS) $(CFLAGS) \
	    -mgeneral-regs-only -c -o $@ $<

clean:
	rm -rf $(OBJS) kernel.elf
