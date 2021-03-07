#!/bin/sh
set -e
. ./config.sh

qemu-system-x86_64 -drive if=pflash,format=raw,file=$EXTERNAL/OVMF.fd \
    -drive format=raw,file=fat:rw:$HDDDIR -M accel=kvm:tcg \
    -net none -serial stdio
