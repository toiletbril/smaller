@echo off

if "%1" == "nogui" (
    set DIALOG=-DNO_DIALOG
) else if "%1" == "gui" (
    @rem Nothing
) else (
    echo USAGE: build ^<gui/nogui^>
    exit 1
)

if "%2" neq "debug" (
    set OPTIMIZE=-Os
)

if not exist bin mkdir bin

set CC=clang
set FLAGS=-Wall -Wextra -Wno-sign-compare -Wno-unused -Wno-deprecated-declarations -Iexternal
set LIBS=-lcomdlg32
set FILES=src\main.c src\cli.c src\smaller.c src\gui.c

@echo on
%CC% %DIALOG% %OPTIMIZE% %FLAGS% %FILES% -o bin\smaller.exe %LIBS%
