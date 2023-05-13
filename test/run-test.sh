#!/bin/bash

if [ -z $LLVM_CONFIG ];
then
	export LLVM_CONFIG=llvm-config
fi

LLVM_BINDIR=$($LLVM_CONFIG --bindir 2>/dev/null)
CC=$LLVM_BINDIR/clang
CXX=$LLVM_BINDIR/clang++

LIB_PATH=$(realpath $(dirname $(realpath "${BASH_SOURCE[0]}"))/../build/lib)

echo $LIB_PATH

$CC -Xclang -load -Xclang $LIB_PATH/libLACommenter.so -Xclang -add-plugin -Xclang LAC test-lacommenter.c -c -o /dev/null