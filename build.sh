#!/bin/bash

if pkg-config --exists 'gtk+-3.0'; then
    GTK_FLAGS="$(pkg-config --cflags 'gtk+-3.0')"
    GTK_LIBS="$(pkg-config --libs 'gtk+-3.0')"
else
    echo "WARNING: gtk+-3.0 is not found, using -DNO_DIALOG."
    GTK_FLAGS="-DNO_DIALOG"
fi

FLAGS="-Os -Wall -Wextra -Iexternal $GTK_FLAGS"
LIBS="-lm $GTK_LIBS"
FILES="src/main.c src/cli.c src/smaller.c src/gui.c"

mkdir -p bin

set -xe

cc $FLAGS $FILES -o bin/smaller $LIBS
