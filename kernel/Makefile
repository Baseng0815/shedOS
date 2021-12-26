CC 		:= cc
AS 		:= as
LD 		:= ld
AR 		:= ar

# 1920x1080: FB_FHD
# 2560x1440: FB_QHD
FB_SIZE 	:= FB_FHD

CFLAGS	:=\
    	$(USER_CFLAGS) \
	-I.\
	-std=gnu11 \
	-ffreestanding \
	-fno-stack-protector \
	-fno-pic \
	-mno-80387 \
	-mno-mmx \
	-mno-3dnow \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-mcmodel=kernel \
	-MMD \
    	-D$(FB_SIZE)

LDFLAGS :=\
    	-Tlinker.ld \
    	-nostdlib \
    	-z max-page-size=0x1000 \
	-static \

ASFLAGS 	:=

SOURCES 	:= $(shell find ./src -type f -name "*.c")
ASM_SOURCES 	:= $(shell find ./src -type f -name "*.S")
HEADERS 	:= $(shell find ./src -type f -name "*.h")
OBJS 		:= $(SOURCES:.c=_c.o) $(ASM_SOURCES:.S=_S.o)
HEADER_DEPS	:= $(SOURCES:.c=.d)

.PHONY: clean install-header install-exec

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
	$(LD) $(OBJS) $(LDFLAGS) -o $@

%_c.o: %.c
	@echo "=====! COMPILING $< INTO $@ !====="
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

%_S.o: %.S
	@ echo =====! ASSEMBLING $< =====!
	$(AS) $(ASFLAGS) -o $@ $<

src/interrupts/interrupts_c.o: src/interrupts/interrupts.c
	@echo "=====! COMPILING $< INTO $@ !====="
	$(CC) $(CPPFLAGS) $(CFLAGS) \
	    -mgeneral-regs-only -c -o $@ $<

clean:
	rm -rf $(OBJS) $(HEADER_DEPS) kernel.elf
