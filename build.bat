@echo off

set CC=clang
set FLAGS=-O2 -Oz -Wall -pedantic

if not exist bin mkdir bin

@echo on
%CC% %FLAGS% smaller.c -o bin/smaller.exe
