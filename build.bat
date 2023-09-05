@echo off

set CC=clang
set FLAGS=-Os -Wall -Wextra -Wno-sign-compare -Wno-unused -Wno-deprecated-declarations -Iexternal
set LIBS=-lcomdlg32
set FILES=src/main.c src/cli.c src/smaller.c src/gui.c

if not exist bin mkdir bin

@echo on

%CC% %FLAGS% %FILES% -o bin/smaller.exe %LIBS%
