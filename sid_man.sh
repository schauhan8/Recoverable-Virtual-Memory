#!/bin/bash
make clean
make
gcc -g testfile.c lrvm_lib.a -o test
./test
