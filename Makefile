export USER_CFLAGS 		:= -O2 -Wall -Wextra -ggdb
export MAKEOPTS 		:= -j12

export TARGET 			:= x86_64-elf
export PROJECTS 		:= kernel
export SYSTEM_HEADER_PROJECTS 	:= kernel
export SYSROOT 			:= $(shell pwd)/sysroot
export ISO 			:= kernel.iso
export DEPENDENCIES 		:= $(shell pwd)/deps
export TOOLCHAIN 		:= $(shell pwd)/toolchain
export PATH 			:= $(TOOLCHAIN)/bin:$(PATH)

.PHONY: all clean sysroot toolchain toolchain-clean

qemu: iso
	qemu-system-x86_64 \
	    -drive if=pflash,format=raw,file=$(DEPENDENCIES)/OVMF.fd \
	    -cdrom $(ISO) \
	    -enable-kvm \
	    -cpu host \
	    -net none \
	    -serial stdio -s -m 512M

iso: sysroot
	rm -f $(ISO)
	cp $(DEPENDENCIES)/limine/limine-cd.bin $(SYSROOT)/boot
	cp $(DEPENDENCIES)/limine/limine-eltorito-efi.bin $(SYSROOT)/boot
	cp $(DEPENDENCIES)/limine/limine.sys $(SYSROOT)/boot
	cp limine.cfg $(SYSROOT)/boot
	xorriso -as mkisofs -eltorito-alt-boot -e boot/limine-eltorito-efi.bin \
	    -no-emul-boot $(SYSROOT) -o $(ISO)

sysroot:
	@for project in $(SYSTEM_HEADER_PROJECTS); do \
	    make -C $$project install-headers || exit 3; \
	    done
	@for project in $(PROJECTS); do \
	    make -C $$project install-exec || exit 3; \
	    done
	mkdir -p $(SYSROOT)/efi/boot
	cp $(DEPENDENCIES)/limine/BOOTX64.EFI $(SYSROOT)/efi/boot

clean:
	@for project in $(PROJECTS); do \
	    make -C $$project clean; \
	    done
	rm -rf $(SYSROOT) $(ISO)

# build a generic x86_64-elf gcc cross toolchain with -mno-red-zone libgcc
toolchain:
	make -C $(TOOLCHAIN)

toolchain-clean:
	make -C $(TOOLCHAIN) clean
