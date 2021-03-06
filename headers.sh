#!/bin/sh
set -e
. ./config.sh

for project in $SYSTEM_HEADER_PROJECTS; do
    (cd $project && make install-headers)
done
