#!/bin/sh
set -e
. ./config.sh

qemu-system-x86_64 -bios $EXTERNAL/bios64.bin -cdrom $DESTDIR/cdimage.iso
