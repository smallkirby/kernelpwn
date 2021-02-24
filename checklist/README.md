# Checklist
This dir contains a checklist when solving kernel-pwn challs. The content contains the list of what you can do under the specific situation (e.g. when SMAP/SMEP/FGKASLR are enabled, when you have AAR, when you have AAW but SLUB is broken, etc...) and the list of you have to check when you somehow can't pwn the kernel remotely.  **You might feel them really something petty**, but it is important for me cuz I really hate the situation where one small typo in my exploit ruins the whole thing, then I notice it after the CTF is over...  
In addition, this dir serves as a reference to [technique dir](../technique/), which explains when, how, why you can use the technique.
  
## Foremost, check the security/config of the kernel
- SMEP / SMAP / KASLR / KAISER
  - just check a run-script of QEMU.
- FGKASLR
  - extract `vmlinux` and do `readelf -S` or just `file`. If FGKASLR is not effective, the num of sections would be 30+. If FGKASLR is enabled, `vmlinux` would have 30000+ sections for each funcs.
  - ```fgkaslr-sections-example.txt
    # FGKASLR is disabled
      ./vmlinux: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), statically linked, BuildID[sha1]=db725bd7b3223f4526170dcdf997a069aba00508, stripped
      [27] .data_nosave      PROGBITS         ffffffff81d65000  01165000
       0000000000000000  0000000000000000   W       0     0     1
      [28] .bss              NOBITS           ffffffff81d65000  01165000
       000000000009b000  0000000000000000  WA       0     0     4096
      [29] .brk              NOBITS           ffffffff81e00000  01165000
       000000000002c000  0000000000000000  WA       0     0     1
      [30] .shstrtab         STRTAB           0000000000000000  01165000
       0000000000000153  0000000000000000           0     0     1

    # FGKASLR is enabled
      ./vmlinux: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), too many section (36140)
        [36135] .brk              NOBITS           ffffffff82a00000  01870000
       000000000002c000  0000000000000000  WA       0     0     1
        [36136] .init.scratch     PROGBITS         ffffffff82c00000  02000000
            0000000000400000  0000000000000000  WA       0     0     32
        [36137] .symtab           SYMTAB           0000000000000000  02400000
            00000000000d3a40  0000000000000018          36138   36107     8
        [36138] .strtab           STRTAB           0000000000000000  024d3a40
            00000000000000ce  0000000000000000           0     0     1
        [36139] .shstrtab         STRTAB           0000000000000000  024d4590
            00000000000ece8d  0000000000000000           0     0     1
    ```
- `CONFIG_SLAB_FREELIST_HARDEN`
  - If this config is enabled, the pointer in free objects in `freelist` is obfuscated by xoring with `random` value of `kmem_cache` structure.
  - To check, first set a breakpoint on `kmme_cache_alloc()`, `slab_alloc()` or something related to slub allocation. Then, determine the addr of `kmem_cache` and calculate the addr of `kmem_cache_cpu` by adding the offset of `kmem_cache.cpu_slab` to $GS_BASE, then you would know the addr `freelist` is pointing to. After that, check the free objects in the slub and the pointer in it is valid addr or not.

- `CONFIG_SLAB_FREELIST_RANDOM`
  - TODO
  
- JIT for eBPF is enabled
  - Since the disclosure of meltdown/spectre, the option to use JIT for eBPF in any case is added to kernel config. To check if JIT is enabled, just `cat /proc/sys/net/core/bpf_jit_enable` (needs root permission). 0 means *disabled*, 1 means *enabled*, and 2 means *debug-mode* where dmesg exposes the result of JIT-compile.
  - This is not related to kernel-pwn, but I felt it funny. eBPF interpreter was abused to realise Spectre v2. So `CONFIG_BPF_JIT_ALWAYS_ON` is added. However, JITed code was also abused to realise Spectre v1...

- type of SLAB allocator
  - TODO

- Oops to panic or not
  - just check a run-script of QEMU. If you see `-append "oops=panic"`, oops causes panic.
  - If oops doesn't mean panic and oops just kills the ill process, you can abuse it. You can cause oops and check kernel-log to leak stack-pointer, kernbase, or kernheap.

## You have AAR
TODO

## You have kUAF
TODO

## You have 
TODO
  
## Exploit works locally, but doesn't work in the remote environment
- **check the result of all functions and systemcalls.**
  - Error handling is always important as you may know. But in exploit code, I sometimes be too lazy to check all the return value of functions. If some funcs fail due to the difference between local/remote environments, it can collapse the exploit.
  - **Example**: I tried to abuse kUAF by re-allocating a chunk by `setxattr()`. In local, I put my exploit in `/tmp/exploit`, so I did `setxattr("/tmp/exploit", "NIRUGIRI", buf, size, XATTR_CREATE)`. But in the remote, `/tmp` was mounted as NX then I put it in `/home/user/exploit`. As a result, `setxattr()` failed due to non-existance of the file but I couldn't notice it. What I have to do was only to put `setxattr` in `assert()`...
    - As an execuse, I usually wrap all funcs/syscalls in `assert` in my exploit. However in the CTF, I abused a race condition and the thread have to do his business as quickly as possible. So I didn't wrap it and things went totally bad...  In addition, I couldn't doubt `setxattr` cuz the only reason of failure should've been related to race.   
  
- **check whether the race can be more stable**
    - In general, race condition can be stable by ordinaly `userfaultfd()` technique. However, uffd can be disable by kernel config `CONFIG_USERFAULTFD`. In this case, you have choices of forcing race by many tries, or finding other vulns. If you choose the former, you have to find the way where the race becomes more stable.  
      - For example, heavy kernel functions are good victim. `_copy_from_user()` and `_copy_to_user()` is usually good even if uffd is disabled.  In addition, if you **pass them a large large large buffer**, they would take more time to finish and the success rate of the race would be greater.