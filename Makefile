export USER_CFLAGS 		:= -O0 -Wall -Wextra -ggdb -pipe
export MAKEOPTS 		:= -j12

export TARGET 			:= x86_64-elf
export PROJECTS 		:= kernel
export SYSTEM_HEADER_PROJECTS 	:= kernel
export SYSROOT 			:= $(shell pwd)/sysroot
export HDD 			:= kernel.hdd
export USB 			:= kernel_usb.hdd
export DEPENDENCIES 		:= $(shell pwd)/deps
export TOOLCHAIN 		:= $(shell pwd)/toolchain
export PATH 			:= $(TOOLCHAIN)/bin:$(PATH)

QEMU_MEMORY 			:= 2G
QEMU_FLAGS  			?=

.PHONY: all clean sysroot toolchain toolchain-clean

qemu: hdd
	qemu-system-x86_64 \
	    -hda $(HDD) \
	    -enable-kvm \
	    -vga std \
	    -soundhw pcspk \
	    -cpu host \
	    -smp $$(nproc) \
	    -net none \
	    -M q35 \
	    -drive file=./disk_image.img,if=none,id=nvme0 \
	    -device nvme,drive=nvme0,serial=deadbeaf1,num_queues=8 \
	    -no-shutdown -no-reboot \
	    -serial stdio -s -m $(QEMU_MEMORY) $(QEMU_FLAGS)

hdd: sysroot
	@echo "=====! CREATING HDD FOR QEMU !====="
	rm -f $(HDD)
	dd if=/dev/zero bs=1M count=0 seek=64 of=$(HDD)
	parted -s $(HDD) mklabel gpt
	parted -s $(HDD) mkpart primary 2048s 100%
	echfs-utils -g -p0 $(HDD) quick-format 512
	@for f in $(shell cd $(SYSROOT) && find . -type f | cut -c 3-); do \
	    echo "Importing $$f..."; \
	    echfs-utils -g -p0 $(HDD) import $(SYSROOT)/$$f $$f; \
	    done
	$(DEPENDENCIES)/limine/limine-install $(HDD)

# mounting it is easier than using mmd for the time being
usb: sysroot
	@echo "=====! CREATING HDD FOR USB BOOTING !====="
	rm -f $(USB)
	dd if=/dev/zero bs=1M count=256 of=$(USB)
	mkfs.fat -F32 $(USB)
	mkdir -p $(USB)_mnt
	mount $(USB) $(USB)_mnt
	cp -r $(SYSROOT)/* $(USB)_mnt
	umount $(USB)
	rmdir $(USB)_mnt

sysroot:
	@echo "=====! CREATING SYSROOT !====="
	@for project in $(SYSTEM_HEADER_PROJECTS); do \
	    make -C $$project install-headers || exit 3; \
	    done
	@for project in $(PROJECTS); do \
	    make -C $$project install-exec || exit 3; \
	    done
	mkdir -p $(SYSROOT)/EFI/BOOT
	cp limine.cfg $(SYSROOT)/boot
	cp $(DEPENDENCIES)/limine/limine.sys $(SYSROOT)/boot
	cp $(DEPENDENCIES)/limine/BOOTX64.EFI $(SYSROOT)/EFI/BOOT

clean:
	@for project in $(PROJECTS); do \
	    make -C $$project clean; \
	    done
	rm -rf $(SYSROOT) $(HDD) $(USB)

# build a generic x86_64-elf gcc cross toolchain with -mno-red-zone libgcc
toolchain:
	make -C $(TOOLCHAIN)

toolchain-clean:
	make -C $(TOOLCHAIN) clean
