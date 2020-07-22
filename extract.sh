#!/bin/sh

sudo rm -r ./extracted
mkdir extracted
cd extracted
cpio -idv < ../rootfs.cpio
cd ../
