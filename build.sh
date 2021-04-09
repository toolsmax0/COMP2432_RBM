#!/bin/sh
mkdir out -p
gcc -g -o out/RBM src/*.c src/lib/*.c -std=c11