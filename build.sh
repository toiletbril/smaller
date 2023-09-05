#!/bin/bash

OPTIMIZE="-Os"

case $1 in
   "auto")
        if pkg-config --exists 'gtk+-3.0'; then
            GTK_FLAGS="$(pkg-config --cflags 'gtk+-3.0')"
            GTK_LIBS="$(pkg-config --libs 'gtk+-3.0')"
        else
            echo "gtk+-3.0 is not found, using -DNO_DIALOG."
            GTK_FLAGS="-DNO_DIALOG"
        fi
        ;;
    "gui")
        GTK_FLAGS="$(pkg-config --cflags 'gtk+-3.0')"
        GTK_LIBS="$(pkg-config --libs 'gtk+-3.0')"
        ;;
    "nogui")
        GTK_FLAGS="-DNO_DIALOG"
        ;;
    *)
        echo "USAGE: build.sh <auto/gui/nogui>"
        exit 1
        ;;
esac

if [ "$2" == "debug" ]; then
    OPTIMIZE=""
fi

FLAGS="$OPTIMIZE -Wall -Wextra -Iexternal $GTK_FLAGS"
LIBS="-lm $GTK_LIBS"
FILES="src/main.c src/cli.c src/smaller.c src/gui.c"

mkdir -p bin

set -xe

cc $FLAGS $FILES -o bin/smaller $LIBS
