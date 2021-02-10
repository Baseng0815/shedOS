#!/bin/sh
set -e
. ./config.sh

qemu-system-x86_64 -bios $BIOS_BIN -cdrom $DESTDIR/cdimage.iso
