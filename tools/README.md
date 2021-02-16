# tools
This dir contains tools/templates and explanations about them.

## contents
### templates
Template exploit code for kernel-pwn.
- [exploit.c](https://github.com/smallkirby/snippet/blob/master/exploit/kernel/exploit.c)
  - General template
- [userfaultfd.c](https://github.com/smallkirby/snippet/blob/master/exploit/kernel/userfaultfd.c)
  - template for exploit using classic uffd technique.
- [ebpf.c](https://github.com/smallkirby/snippet/blob/master/exploit/kernel/bpf-exploit.c)
  - template for exploit using eBPF exploit. Generally used macros are defined.

### snippet
- [mr.sh](https://github.com/smallkirby/snippet/blob/master/exploit/kernel/mr.sh)
  - stands for *My Run-script*.
  - extract rootfs/initramfs, compile exploit, copy exploit into the filesystem, and re-compile it, then boot kernel on QEMU with debug-friendly config.
- [extract.sh](https://github.com/smallkirby/snippet/blob/master/exploit/kernel/extract.sh)
  - extract rootfs/initramfs.
- [extract-vmlinux.sh](https://github.com/smallkirby/snippet/blob/master/exploit/kernel/extract-vmlinux.sh)
  - extract bzImage/vmlinuz into vmlinux. This script is cited from [here](https://github.com/torvalds/linux).
- [sender.py](https://github.com/smallkirby/snippet/blob/master/exploit/kernel/sender.py)
  - send gziped/base64ed exploit binary to the host.