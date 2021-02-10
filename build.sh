#!/bin/sh
set -e
. ./config.sh

for project in $PROJECTS; do
    (cd $project && make install)
done
