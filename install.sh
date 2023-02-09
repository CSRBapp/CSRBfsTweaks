#!/bin/bash -x

mkdir "$1"
cp -p CSRBfsTweaks.so run.sh "$1/"

#cat /lib/x86_64-linux-gnu/libc.so.6 > "$1/libc.so.6"
#cat /lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 > "$1/ld-linux-x86-64.so.2"
#cat /lib/x86_64-linux-gnu/libpthread.so.0 > "$1/libpthread.so.0"
#cat /lib/x86_64-linux-gnu/libdl.so.2 > "$1/libdl.so.2"

