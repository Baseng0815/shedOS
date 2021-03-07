EXTERNAL?=
EFI_FILE=bootx64.efi

# using LLVM/Clang because it natively supports cross-compilation
# (and because I can't get a w64-mingw32 cc to work properly. Too bad!)
CC:=clang

CFLAGS:=-target x86_64-unknown-windows \
    -ffreestanding \
    -fshort-wchar \
    -mno-red-zone \
    -I$(EXTERNAL)/gnuefi/inc -I$(EXTERNAL)/gnuefi/inc/x86_64 \
    -I$(EXTERNAL)/gnuefi/inc/protocol \
    -I$(EXTERNAL)/include

LDFLAGS:=-target x86_64-unknown-windows \
    -nostdlib \
    -Wl,-entry:efi_main \
    -Wl,-subsystem:efi_application \
    -fuse-ld=lld-link

OBJS:=src/main.o \
    data.o

.PHONY: all clean install

all: install

install: $(EFI_FILE)
	mkdir -p $(SYSROOT)/EFI/BOOT
	cp -f $(EFI_FILE) $(SYSROOT)/EFI/BOOT

$(EFI_FILE): $(OBJS)
	$(CC) $(LDFLAGS) -o $(EFI_FILE) $(OBJS)

# this is needed by GNU-EFI
data.o: $(EXTERNAL)/gnuefi/data.c
	$(CC) $(CFLAGS) -std=gnu11 -c -o data.o $(EXTERNAL)/gnuefi/data.c

%.o: %.c
	$(CC) $(CFLAGS) -std=gnu11 -c -o $@ $<

clean:
	rm -rf $(OBJS) $(EFI_FILE) data.o
