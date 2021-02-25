EXTERNAL?=

# using LLVM/Clang because it natively supports cross-compilation
# (and because I can't get a w64-mingw32 cc to work properly. Too bad!)
CC:=clang

CFLAGS:=-target x86_64-unknown-windows \
    -ffreestanding \
    -fshort-wchar \
    -mno-red-zone \
    -I$(EXTERNAL)/gnuefi/inc -I$(EXTERNAL)/gnuefi/inc/x86_64 \
    -I$(EXTERNAL)/gnuefi/inc/protocol \
    -Iinclude

LDFLAGS:=-target x86_64-unknown-windows \
    -nostdlib \
    -Wl,-entry:efi_main \
    -Wl,-subsystem:efi_application \
    -fuse-ld=lld-link

OBJS:=src/main.o \
    data.o

.PHONY: all clean install

all: install

install: EFI
	mkdir -p $(DESTDIR)
	cp -f BOOTX64.EFI $(DESTDIR)

EFI: $(OBJS)
	$(CC) $(LDFLAGS) -o BOOTX64.EFI $(OBJS)

# this is needed by GNU-EFI
data.o: $(EXTERNAL)/gnuefi/data.c
	$(CC) $(CFLAGS) -std=gnu11 -c -o data.o $(EXTERNAL)/gnuefi/data.c

%.o: %.c
	$(CC) $(CFLAGS) -std=gnu11 -c -o $@ $<

clean:
	rm -rf $(OBJS) BOOTX64.EFI fat.img iso cdimage.iso data.o
