#!/bin/bash

#export CSRBFSTWEAKSDEBUG=1

#TWEAKSDIR=$(dirname $(realpath -s $0))
TWEAKSDIR=/CSRBfsTweaks/

export LD_BIND_NOW=BIND

export LD_PRELOAD=$TWEAKSDIR/CSRBfsTweaks.so

#export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH
#export LD_PRELOAD=$(dirname $0)/ld-linux-x86-64.so.2:$(dirname $0)/CSRBfsTweaks.so

#echo LD_PRELOAD=$LD_PRELOAD

$*

