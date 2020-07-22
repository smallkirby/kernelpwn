#!/bin/sh
qemu-system-x86_64 \
    -m 256M \
    -kernel ./pure/bzImage \
    -initrd ./myrootfs.cpio \
    -append "root=/dev/ram rw console=ttyS0 oops=panic panic=1 quiet" \
    -cpu kvm64,+smep,+smap \
    -monitor /dev/null \
    -nographic -enable-kvm \
    -s
