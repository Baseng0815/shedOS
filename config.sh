#!/bin/sh
export CC=clang
export AR=ar

export PROJECTS="libc kernel loader"
export SYSTEM_HEADER_PROJECTS="libc kernel"
export SYSROOT=$(pwd)/sysroot
export ISODIR=$(pwd)/iso
export EXTERNAL=$(pwd)/external

export CC="$CC --sysroot=$SYSROOT -isystem=include"
