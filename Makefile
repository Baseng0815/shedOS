export USER_CFLAGS 		:= -O0 -Wall -Werror -Wextra -Wno-missing-field-initializers -ggdb -pipe
export MAKEOPTS 		:= -j12

export TARGET 			:= x86_64-elf
export PROJECTS 		:= kernel
export SYSTEM_HEADER_PROJECTS 	:= kernel
export SYSROOT 			:= $(CURDIR)/sysroot
export ISO 			:= $(CURDIR)/kernel.iso
export DEPENDENCIES 		:= $(CURDIR)/deps
export TOOLCHAIN 		:= $(CURDIR)/toolchain
export PATH 			:= $(TOOLCHAIN)/bin:$(PATH)

QEMU_MEMORY 			:= 2G
QEMU_FLAGS  			?=

.PHONY: all clean sysroot $(SYSTEM_HEADER_PROJECTS) $(PROJECTS)

qemu: iso
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
	    -no-shutdown -no-reboot \
	    -serial stdio -s -m $(QEMU_MEMORY) $(QEMU_FLAGS)

iso: $(ISO)

$(ISO): sysroot
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
	$(DEPENDENCIES)/limine/limine-install $(ISO)

sysroot: $(SYSTEM_HEADER_PROJECTS) $(PROJECTS)
	@echo "=====! BUILDING DEPENDENCIES !====="
	cd $(DEPENDENCIES) && $(MAKE)
	@echo "=====! CREATING SYSROOT !====="
	cp limine.cfg $(SYSROOT)/boot
	cp $(DEPENDENCIES)/limine/limine.sys $(SYSROOT)/boot
	cp $(DEPENDENCIES)/limine/limine-cd.bin $(SYSROOT)/boot
	cp $(DEPENDENCIES)/limine/limine-eltorito-efi.bin $(SYSROOT)/boot
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
	rm -rf $(SYSROOT) $(ISO)

# build a generic x86_64-elf gcc cross toolchain with -mno-red-zone libgcc
toolchain:
	cd $(TOOLCHAIN) && $(MAKE)

toolchain-clean:
	cd $(TOOLCHAIN) && $(MAKE) clean
