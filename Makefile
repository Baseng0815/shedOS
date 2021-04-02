export USER_CFLAGS 		:= "-O2 -Wall -Wextra -ggdb -std=gnu11"
export MAKEOPTS 		:= "-j12"

export TARGET 			:= x86_64-elf
export PROJECTS 		:= libc kernel loader
export SYSTEM_HEADER_PROJECTS 	:= libc kernel
export SYSROOT 			:= $(shell pwd)/sysroot
export ISODIR 			:= $(shell pwd)/iso
export DEPENDENCIES 		:= $(shell pwd)/deps
export TOOLCHAIN 		:= $(shell pwd)/toolchain
export PATH 			:= $(TOOLCHAIN)/bin:$(PATH)

# export CC 			:= $(TARGET)-gcc --sysroot=$(SYSROOT)
# export AS 			:= $(TARGET)-as
# export LD 			:= $(TARGET)-ld
# export AR 			:= $(TARGET)-ar

.PHONY: toolchain

# build a generic x86_64-elf gcc cross toolchain with -mno-redzone
toolchain:
	cd $(TOOLCHAIN) && make

toolchain-clean:
	cd $(TOOLCHAIN) && make clean
