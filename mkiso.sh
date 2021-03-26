#!/bin/sh
set -e
. ./config.sh

mkdir -p $ISODIR
pushd $ISODIR

# img
dd if=/dev/zero of=fat.img bs=512 count=262144
mformat -i fat.img -f 2880 ::

# create directories
ds=$(cd $SYSROOT; find . -type d | cut -c 3-)
echo $ds
for d in $ds; do
    echo "=====! CREATING DOS DIRECTORY $d !====="
    mmd -i fat.img ::$d
done

# copy files
files=$(cd $SYSROOT; find . -type f | cut -c 3-)
echo $files

for file in $files; do
    echo "=====! COPYING FILE $file !====="
    mcopy -i fat.img $SYSROOT/$file ::$file
done

popd

# iso
xorriso -as mkisofs -R -f -e fat.img -no-emul-boot \
    -o $ISODIR/cdimage.iso $ISODIR
