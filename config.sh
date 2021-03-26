#!/bin/sh
# all those flags DO NOT apply to the bootloader!
export CFLAGS="-O2 -Wall -Wextra -ggdb -std=gnu11"

# we use a generic x86_64 target using the ELF format
export TARGET=x86_64-elf
export CC=$TARGET-gcc
export AS=$TARGET-as
export LD=$TARGET-ld
export AR=$TARGET-ar

export PROJECTS="libc kernel loader"
export SYSTEM_HEADER_PROJECTS="libc kernel"
export SYSROOT=$(pwd)/sysroot
export ISODIR=$(pwd)/iso
export EXTERNAL=$(pwd)/external

# all projects will be gradually installed into the sysroot
export CC="$CC --sysroot=$SYSROOT"
