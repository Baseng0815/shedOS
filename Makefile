export USER_CFLAGS 		:= -O0 -Wall -Wextra -ggdb -pipe
export MAKEOPTS 		:= -j12

export TARGET 			:= x86_64-elf
export PROJECTS 		:= kernel
export SYSTEM_HEADER_PROJECTS 	:= kernel
export SYSROOT 			:= $(CURDIR)/sysroot
export ISO 			:= $(CURDIR)/kernel.iso
export USB 			:= $(CURDIR)/kernel_usb.hdd
export DEPENDENCIES 		:= $(CURDIR)/deps
export TOOLCHAIN 		:= $(CURDIR)/toolchain
export PATH 			:= $(TOOLCHAIN)/bin:$(PATH)

QEMU_MEMORY 			:= 2G
QEMU_FLAGS  			?=

.PHONY: all clean $(SYSTEM_HEADER_PROJECTS) $(PROJECTS)

qemu: hdd
	qemu-system-x86_64 \
	    -boot d \
	    -cdrom $(ISO) \
	    -enable-kvm \
	    -cpu host \
	    -vga std \
	    -soundhw pcspk \
	    -smp $$(nproc) \
	    -net none \
	    -M q35 \
	    -drive file=./disk_image.img,if=none,id=nvme0 \
	    -device nvme,drive=nvme0,serial=deadbeaf1,num_queues=8 \
	    -no-shutdown -no-reboot \
	    -serial stdio -s -m $(QEMU_MEMORY) $(QEMU_FLAGS)

hdd: sysroot
	@echo "=====! CREATING ISO FOR QEMU !====="
	rm -f $(ISO)
	xorriso -as mkisofs \
		-b boot/limine-cd.bin \
		-no-emul-boot \
		-boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine-eltorito-efi.bin \
		-efi-boot-part \
		--efi-boot-image \
		--protective-msdos-label \
		sysroot -o $(ISO)

usb: sysroot
	@echo "=====! CREATING ISO FOR USB BOOTING !====="
	rm -f $(USB)
	dd if=/dev/zero bs=4M count=16 of=$(USB)
	parted -s $(USB) mklabel gpt
	mkfs.fat -F32 $(USB)
	mkdir -p $(USB)_mnt
	mount $(USB) $(USB)_mnt
	cp -r $(SYSROOT)/* $(USB)_mnt
	umount $(USB)
	rmdir $(USB)_mnt

sysroot: $(SYSTEM_HEADER_PROJECTS) $(PROJECTS)
	@echo "=====! CREATING SYSROOT !====="
	mkdir -p $(SYSROOT)/EFI/BOOT
	cp limine.cfg $(SYSROOT)/boot
	cp $(DEPENDENCIES)/limine/limine.sys $(SYSROOT)/boot
	cp $(DEPENDENCIES)/limine/limine-cd.bin $(SYSROOT)/boot
	cp $(DEPENDENCIES)/limine/limine-eltorito-efi.bin $(SYSROOT)/boot
	cp $(DEPENDENCIES)/limine/BOOTX64.EFI $(SYSROOT)/EFI/BOOT
	cp $(DEPENDENCIES)/testelf1 $(SYSROOT)/boot
	cp $(DEPENDENCIES)/testelf2 $(SYSROOT)/boot

$(SYSTEM_HEADER_PROJECTS):
	cd $@ && $(MAKE) install-headers

$(PROJECTS):
	cd $@ && $(MAKE) install-exec

clean:
	@for project in $(PROJECTS); do \
	    cd $$project && $(MAKE) clean; \
	    done
	rm -rf $(SYSROOT) $(ISO) $(USB)

# build a generic x86_64-elf gcc cross toolchain with -mno-red-zone libgcc
toolchain:
	cd $(TOOLCHAIN) && $(MAKE)

toolchain-clean:
	cd $(TOOLCHAIN) && $(MAKE) clean
