#!/bin/bash

FLAGS="-O2 -Oz -Wall -lm"

mkdir -p bin

set -xe
cc $FLAGS smaller.c -o bin/smaller
