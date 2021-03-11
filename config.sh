#!/bin/sh
# all those flags DO NOT apply to the bootloader!
export CC=clang
export LD=ld
# TODO fix 'everything breaks'-bug when using -O>0
export CFLAGS="-O0 -Wall -Wextra -ggdb"

export PROJECTS="libc kernel loader"
export SYSTEM_HEADER_PROJECTS="libc kernel"
export SYSROOT=$(pwd)/sysroot
export HDDDIR=$(pwd)/hdd
export EXTERNAL=$(pwd)/external

export CC="$CC --sysroot=$SYSROOT -isystem=include"
