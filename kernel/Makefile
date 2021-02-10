GNU_EFI?=/home/bastian/kern/gnu-efi
BIOS_BIN?=/home/bastian/kern/bios64.bin

# using LLVM/Clang because it natively supports cross-compilation
# (and because I can't get a w64-mingw32 cc to work properly. Too bad!)
CC:=clang

# maybe make this platform-independent some time in the future
# for now, everything is put into include and src and linked together
ARCH_DIR:=src

CFLAGS:=-target x86_64-unknown-windows \
    -ffreestanding \
    -fshort-wchar \
    -mno-red-zone \
    -I$(GNU_EFI)/inc -I$(GNU_EFI)/inc/x86_64 \
    -I$(GNU_EFI)/inc/protocol \
    -Iinclude

LDFLAGS:=-target x86_64-unknown-windows \
    -nostdlib \
    -Wl,-entry:efi_main \
    -Wl,-subsystem:efi_application \
    -fuse-ld=lld-link

include $(ARCH_DIR)/make.config
OBJS:=$(ARCH_OBJS) data.o

.PHONY: all clean run install

all: install

install: ISO
	mkdir -p $(DESTDIR)
	cp -f cdimage.iso $(DESTDIR)/cdimage.iso

EFI: $(OBJS)
	$(CC) $(LDFLAGS) -o BOOTX64.EFI $(OBJS)

IMG: EFI
	dd if=/dev/zero of=fat.img bs=1k count=1440
	mformat -i fat.img -f 1440 ::
	mmd -i fat.img ::/EFI
	mmd -i fat.img ::/EFI/BOOT
	mcopy -i fat.img BOOTX64.EFI ::/EFI/BOOT

ISO: IMG
	mkdir -p iso
	cp -f fat.img iso
	xorriso -as mkisofs -R -f -e fat.img -no-emul-boot \
	-o cdimage.iso iso

# this is needed by GNU-EFI
data.o: $(GNU_EFI)/lib/data.c
	$(CC) $(CFLAGS) -std=gnu11 -c -o data.o $(GNU_EFI)/lib/data.c

%.o: %.c
	$(CC) $(CFLAGS) -std=gnu11 -c -o $@ $<

clean:
	rm -rf $(OBJS) BOOTX64.EFI fat.img iso cdimage.iso
