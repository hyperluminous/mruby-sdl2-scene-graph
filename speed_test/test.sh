#!/bin/sh

# CC=gcc-mp-7
CC=clang

$CC speed.c -Wall -DINVERT_4x4_SSE -O0 -o sse-O0.exe
$CC speed.c -Wall -DINVERT_4x4_SSE -O2 -o sse-O2.exe
$CC speed.c -Wall -DINVERT_4x4_SSE -Os -o sse-Os.exe
clang speed.c -Wall -DINVERT_4x4_LLVM -O0 -o llvm-O0.exe
clang speed.c -Wall -DINVERT_4x4_LLVM -O2 -o llvm-O2.exe
clang speed.c -Wall -DINVERT_4x4_LLVM -O3 -o llvm-Os.exe
$CC speed.c -Wall -O0 -o c-O0.exe
$CC speed.c -Wall -O2 -o c-O2.exe
$CC speed.c -Wall -Os -o c-Os.exe

mkdir -p ../tmp/speed_test
mv *.exe ../tmp/speed_test
cd ../tmp/speed_test
echo "SSE O0"; ./sse-O0.exe
echo "SSE O2"; ./sse-O2.exe
echo "SSE Os"; ./sse-Os.exe
echo "LLVM O0"; ./llvm-O0.exe
echo "LLVM O2"; ./llvm-O2.exe
echo "LLVM Os"; ./llvm-Os.exe
echo "O0"; ./c-O0.exe
echo "02"; ./c-O2.exe
echo "0s"; ./c-Os.exe
cd -
