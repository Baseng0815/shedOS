#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/shedOS.kernel isodir/boot/shedOS.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "shedOS" {
        multiboot /boot/shedOS.kernel
    }
EOF
grub-mkrescue -o shedOS.iso isodir
