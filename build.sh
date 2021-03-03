#!/bin/sh
set -e
. ./config.sh

PROJECTS=(kernel loader)

for project in ${PROJECTS[@]}; do
    (cd $project && make install)
done

cp -r resources/* $DESTDIR

(
# img
cd $DESTDIR;
dd if=/dev/zero of=fat.img bs=512 count=93750;
mformat -i fat.img -f 1440 ::;
mmd -i fat.img ::/EFI;
mmd -i fat.img ::/EFI/BOOT;
mcopy -i fat.img $DESTDIR/BOOTX64.EFI ::/EFI/BOOT;
mcopy -i fat.img $DESTDIR/kernel.elf ::;
mcopy -i fat.img $DESTDIR/font.psf ::;

# iso
mkdir -p iso;
cp -f fat.img iso;
xorriso -as mkisofs -R -f -e fat.img -no-emul-boot -o cdimage.iso iso
)
