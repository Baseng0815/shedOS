#!/bin/sh
export CC=clang
export LD=ld

export PROJECTS="libc kernel loader"
export SYSTEM_HEADER_PROJECTS="libc kernel"
export SYSROOT=$(pwd)/sysroot
export HDDDIR=$(pwd)/hdd
export EXTERNAL=$(pwd)/external

export CC="$CC --sysroot=$SYSROOT -isystem=include"
