#!/bin/sh
# all those flags DO NOT apply to the bootloader!

# we use a generic x86_64 target using the ELF format

# all projects will be gradually installed into the sysroot
export CC="$CC --sysroot=$SYSROOT"
