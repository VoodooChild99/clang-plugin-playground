#!/bin/bash

DIR=$(dirname "$(realpath "${BASH_SOURCE[0]}")")

"$DIR"/../build/compiler/custom-clang "$DIR"/test-lacommenter.c -c -o /dev/null