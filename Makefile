GNU_EFI?=/home/bastian/kern/gnu-efi
BIOS_BIN?=/home/bastian/kern/bios64.bin

CFLAGS:=-target x86_64-unknown-windows \
    -ffreestanding \
    -fshort-wchar \
    -mno-red-zone \
    -I$(GNU_EFI)/inc -I$(GNU_EFI)/inc/x86_64 \
    -I$(GNU_EFI)/inc/protocol

LDFLAGS:=-target x86_64-unknown-windows \
    -nostdlib \
    -Wl,-entry:efi_main \
    -Wl,-subsystem:efi_application \
    -fuse-ld=lld-link

OBJS:=hello.o data.o

.PHONY: all clean run

# using LLVM/Clang because it natively supports cross-compilation
# (and because I can't get a w64-mingw32 cc to work properly. Too bad!)
all: ISO

run:
	qemu-system-x86_64 -bios $(BIOS_BIN) -cdrom cdimage.iso

EFI: $(OBJS)
	clang $(LDFLAGS) -o BOOTX64.EFI hello.o data.o

IMG: EFI
	dd if=/dev/zero of=fat.img bs=1k count=1440
	mformat -i fat.img -f 1440 ::
	mmd -i fat.img ::/EFI
	mmd -i fat.img ::/EFI/BOOT
	mcopy -i fat.img BOOTX64.EFI ::/EFI/BOOT

ISO: IMG
	mkdir -p iso
	cp fat.img iso
	xorriso -as mkisofs -R -f -e fat.img -no-emul-boot -o cdimage.iso iso

data.o: $(GNU_EFI)/lib/data.c
	clang $(CFLAGS) -c -o data.o $(GNU_EFI)/lib/data.c

%.o: %.c
	clang $(CFLAGS) -c -o hello.o hello.c

clean:
	rm -rf *.o *.img *.iso iso *.bin
