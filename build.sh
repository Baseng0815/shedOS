#!/bin/sh
set -e
. ./config.sh

for project in $PROJECTS; do
    (cd $project && make install)
done

mkdir -p $SYSROOT/res
mkdir -p $ISODIR
cp -r resources/* $SYSROOT/res

(
# img
cd $ISODIR;
dd if=/dev/zero of=fat.img bs=512 count=93750;
mformat -i fat.img -f 1440 ::;
# create directories
ds=$(cd $SYSROOT; find . -type d | cut -c 3-);
echo $ds
for d in $ds; do
    mmd -i fat.img ::$d
done
# copy files
files=$(cd $SYSROOT; find . -type f | cut -c 3-)
echo $files

for file in $files; do
    mcopy -i fat.img $SYSROOT/$file ::$file;
done
)

# iso
xorriso -as mkisofs -R -f -e fat.img -no-emul-boot\
    -o $ISODIR/cdimage.iso $ISODIR
