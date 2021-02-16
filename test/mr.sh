#!/bin/bash

# build
cwd=$(pwd)
make
cp ./hoge.ko ./extracted/home/skb
cp ./hoge.ko <buildroot dir>/output/build/linux-<ver>/
gcc ./test.c -o test -O0 -g --static
cp ./test ./extracted/home/skb

# compress
rm ./rootfs.cpio
chmod 777 -R ./extracted
cd ./extracted
find ./ -print0 | cpio --owner root --null -o -H newc > ../rootfs.cpio
cd ../

# run
qemu-system-x86_64 \
  -kernel ./bzImage \
  -initrd ./rootfs.cpio \
  -nographic \
  -monitor none \
  -cpu qemu64 \
  -append "console=ttyS0 nokaslr panic=1" \
  -no-reboot \
  -s \
  -m 256M
