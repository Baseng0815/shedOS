#!/bin/sh
set -e
. ./config.sh

PROJECTS=(kernel loader)

for project in ${PROJECTS[@]}; do
    (cd $project && make clean)
done

rm -rf $DESTDIR
rm -rf $ISODIR
