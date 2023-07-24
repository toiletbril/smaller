#!/bin/bash

FLAGS="-O2 -Os -Wall -Wextra -Iexternal"

if pkg-config --exists "gtk+-3.0"; then
    USE_GTK="$(pkg-config --cflags --libs "gtk+-3.0")"
else
    echo "WARNING: gtk+-3.0 is not found, using -DNO_DIALOG."
    USE_GTK="-DNO_DIALOG"
fi

LIBS="$USE_GTK -lm"
FILES="src/main.c src/cli.c src/smaller.c src/gui.c"

mkdir -p bin

set -xe

cc $FLAGS $FILES -o bin/smaller $LIBS
