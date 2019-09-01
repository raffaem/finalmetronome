#!/bin/sh

DSTDIR=./linux
DSTFILE=finalmetronome-linux-i386.tar.gz

rm -f -r $DSTDIR
mkdir $DSTDIR

cp -f ../build/linux/src/finalmetronome $DSTDIR

cp -f -R ./data 	$DSTDIR
cp -f ../README 	$DSTDIR
cp -f ../AUTHORS 	$DSTDIR
cp -f ../COPYING 	$DSTDIR
cp -f ../ChangeLog 	$DSTDIR
cp -f ../LINUX_BIN_README 	$DSTDIR
cp -f /usr/local/lib/libfmodex-`../build/linux/fmodgetver/fmodgetver`.so $DSTDIR

tar -caf $DSTFILE $DSTDIR
rm -f -r $DSTDIR
