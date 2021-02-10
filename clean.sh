#!/bin/sh
set -e
. ./config.sh

for project in $PROJECTS; do
    (cd $project && make clean)
done

rm -rf $DESTDIR
