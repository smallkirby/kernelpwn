# Module Testing Example
This dir is for quickly debugging module and kernel with debug-info.

## contents
- [hoge.c](./hoge.c)
  - Sample module source. It registers miscdevice and just repeats `kmalloc`. Change as you like.
- [test.c](./test.c)
  - Sample userland program source. It just open miscdevice registered by `hoge.c`. Change as you like.
- [mr.sh](./mr.sh)
  - build module, build userland program, copy them on the filesystem, re-build the filesystem, and boot the kernel on QEMU with debug-friendly config.
- [Makefile](./Makefile)
  - Makefile for the module. It makes it on the buildroot dir.
- [rootfs.cpio](./rootfs.cpio)
  - Example rootfs. It can be extracted and customized as you like by [mr.sh](./mr.sh).
- [extract.sh](./extract.sh)
  - Extract rootfs.cpio.
- [bzImage](./bzImage)
  - Example kernel image from v 5.9.18. You can change it to your image of any version as you like.
