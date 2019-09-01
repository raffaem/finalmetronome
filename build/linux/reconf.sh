#!/bin/sh
# For this script to work, it must be executed from this directory, the same directory where the script resides.
# Executing this script from another directory won't work

make clean
cd ../..
#the --install option will copy auxiliary missing files
autoreconf --install
cd build/linux
../../configure
make
