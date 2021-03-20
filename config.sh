#!/bin/sh
# all those flags DO NOT apply to the bootloader!
# TODO fix 'everything breaks'-bug when using -O>0
export CFLAGS="-O0 -Wall -Wextra -ggdb"

# we use a generic x86_64 target using the ELF format
export TARGET=x86_64-elf
export CC=$TARGET-gcc
export LD=$TARGET-ld
export AR=$TARGET-ar

export PROJECTS="libc kernel loader"
export SYSTEM_HEADER_PROJECTS="libc kernel"
export SYSROOT=$(pwd)/sysroot
export HDDDIR=$(pwd)/hdd
export EXTERNAL=$(pwd)/external

# all projects will be gradually installed into the sysroot
export CC="$CC --sysroot=$SYSROOT"
