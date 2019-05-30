#!/bin/bash
mkdir my_mcce
cp mcce3.0/*.c my_mcce/

cd mcce3.0/lib
make clean
make
cd ..
make
