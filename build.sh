#!/bin/sh
set -e
. ./config.sh

PROJECTS=(kernel loader)

for project in ${PROJECTS[@]}; do
    (cd $project && make install)
done

mkdir -p $DESTDIR/res
mkdir -p $ISODIR
cp -r resources/* $DESTDIR/res

(
# img
cd $ISODIR;
dd if=/dev/zero of=fat.img bs=512 count=93750;
mformat -i fat.img -f 1440 ::;
# create directories
ds=$(cd $DESTDIR; find . -type d | cut -c 3-);
echo $ds
for d in $ds; do
    mmd -i fat.img ::$d
done
# copy files
files=$(cd $DESTDIR; find . -type f | cut -c 3-)
echo $files

for file in $files; do
    mcopy -i fat.img $DESTDIR/$file ::$file;
done
)

# iso
xorriso -as mkisofs -R -f -e fat.img -no-emul-boot\
    -o $ISODIR/cdimage.iso $ISODIR
