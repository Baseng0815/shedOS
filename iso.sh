#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/shedos.kernel isodir/boot/shedos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "shedos" {
        multiboot /boot/shedos.kernel
    }
EOF
grub-mkrescue -o shedos.iso isodir
