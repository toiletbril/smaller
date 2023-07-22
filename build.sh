#!/bin/bash

CC="clang"
FLAGS="-O2 -Os -Wall -Wextra -Iexternal"
LIBS="$(pkg-config --cflags --libs gtk+-3.0) -lm"
FILES="src/main.c src/cli.c src/smaller.c src/gui.c"

mkdir -p bin

set -xe

$CC $FLAGS $LIBS $FILES -o bin/smaller
