#!/bin/sh
set -e
. ./config.sh

for project in $PROJECTS; do
    echo "=====! BUILDING PROJECT $project !====="
    (cd $project && make install)
done

mkdir -p $SYSROOT/res
cp -r resources/* $SYSROOT/res

mkdir -p $HDDDIR
cp -r $SYSROOT/* $HDDDIR
