export USER_CFLAGS 		:= -O0 -Wall -Wextra -ggdb -pipe
export MAKEOPTS 		:= -j12

export TARGET 			:= x86_64-elf
export PROJECTS 		:= kernel
export SYSTEM_HEADER_PROJECTS 	:= kernel
export SYSROOT 			:= $(shell pwd)/sysroot
export HDD 			:= kernel.hdd
export DEPENDENCIES 		:= $(shell pwd)/deps
export TOOLCHAIN 		:= $(shell pwd)/toolchain
export PATH 			:= $(TOOLCHAIN)/bin:$(PATH)

QEMU_MEMORY 			:= 2G

.PHONY: all clean sysroot toolchain toolchain-clean

qemu: $(HDD)
	qemu-system-x86_64 \
	    -hda $(HDD) \
	    -enable-kvm \
	    -vga std \
	    -cpu host \
	    -smp $$(nproc) \
	    -net none \
	    -M q35 \
	    -serial stdio -s -m $(QEMU_MEMORY)

qemu-debug: $(HDD)
	qemu-system-x86_64 \
	    -hda $(HDD) \
	    -enable-kvm \
	    -vga std \
	    -cpu host \
	    -net none \
	    -M q35 \
	    -serial stdio -s -m $(QEMU_MEMORY) -S

$(HDD): sysroot
	rm -f $(HDD)
	dd if=/dev/zero bs=1M count=0 seek=64 of=$(HDD)
	parted -s $(HDD) mklabel gpt
	parted -s $(HDD) mkpart primary 2048s 100%
	echfs-utils -g -p0 $(HDD) quick-format 512
	@for f in $(shell cd $(SYSROOT) && find . -type f | cut -c 3-); do \
	    echo "Importing $$f..."; \
	    echfs-utils -g -p0 $(HDD) import $(SYSROOT)/$$f $$f; \
	    done
	$(DEPENDENCIES)/limine/limine-install-linux-x86_64 $(HDD)

sysroot:
	@for project in $(SYSTEM_HEADER_PROJECTS); do \
	    make -C $$project install-headers || exit 3; \
	    done
	@for project in $(PROJECTS); do \
	    make -C $$project install-exec || exit 3; \
	    done
	mkdir -p $(SYSROOT)/EFI/BOOT
	cp limine.cfg $(SYSROOT)/boot
	cp $(DEPENDENCIES)/limine/limine.sys $(SYSROOT)/boot

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
