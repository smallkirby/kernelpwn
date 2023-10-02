# kernel pwn
## About this repo
This repository collects CTF kernel-pwn challenges and writeups.
Also, it introduces how to start learning kernel-pwn for beginners **including me**.
All the challs here are solved by me, though the writeup may be based on the author's one or others's ones.
I'm planning to include not only kernel-pwn, but also general non-userland pwn including QEMU, V8, multi-arch...

**IMPORTANT**:
**If you know some good kernel(non-userland) pwn challs, please tell me and I'll solve it. Then I'll add it on this repo if I feel it good to solve.**
And if you notice some wrong points in my writeups or blog posts, feel free to [contact me](https://twitter.com/smallkirby_).


## Good Challs

- [corjail](https://github.com/smallkirby/pwn-writeups/tree/master/cor2022/corjail) from [CoRCTF2022](https://github.com/Crusaders-of-Rust/corCTF-2022-public-challenge-archive/tree/master/pwn/corjail/task/build)
  - SMEP/ SMAP/ KPTI/ KASLR/ Docker escape/ heap randomized/ heap hardened/ uffd disabled/ modprobe_path static/ `poll_list`/ `user_key_payload`/ kROP on `tty_struct`
  - https://blog.smallkirby.com/posts/corjail/

- [Fire of Salvation](https://github.com/smallkirby/pwn-writeups/tree/master/cor2021/fire_of_salvation/work) from [CoRCTF2021](https://github.com/Crusaders-of-Rust/corCTF-2021-public-challenge-archive/tree/main/pwn/fire-of-salvation)
  - SMEP/ SMAP/ KPTI/ KASLR/ FGKASLR/ heap randomized/ `msg_msg`/ uffd
  - https://blog.smallkirby.com/posts/fire-of-salvation/

- [nightclub](https://github.com/smallkirby/pwn-writeups/tree/master/pb2021/nightclub/work) from pbctf 2021
  - SMEP/ SMAP/ KPTI/ KASLR/ uffd disabled/ heap
  - https://blog.smallkirby.com/posts/nightclub/

- [kone_gadget](https://github.com/smallkirby/pwn-writeups/tree/master/seccon2021/kone_gadget/work) from SECCON CTF 2021
  - SMEP/ SMAP/ KPTI/ no-KASLR/ unprivileged ebpf installation is NOT allowed / well-crafted shellcode in JIT-ed seccomp filter
  - https://blog.smallkirby.com/posts/kone_gadget/
  - you are able to control RIP but all the registers are zero-cleared before it. Unprivileged ebpf is forbidden, but seccomp filter is still JIT-ed. you abuse operand to be shellcode and jump everytime to next opcode (which means next shellcode).
  - Unexpected solution is `jmp &flag`, which leaks secret content in `CODE` secition of panic message. It means that if the kernel uses `initramfs` and KASLR is disabled (or we have full knowlege of memory layout), just getting RIP means success of exploit, SMEP/SMAP is useless.

- [klibrary](https://github.com/smallkirby/pwn-writeups/tree/master/3k2021/klibrary/work) from 3kCTF 2021
  - SMEP/ SMAP/ KPTI/ KASLR/ race w/ uffd/ `tty_struct`/ `modprobe_overwrite` via vtable in `tty_struct`
  - https://blog.smallkirby.com/posts/klibrary/
  - between beginner and med

- [shared knote](https://github.com/smallkirby/pwn-writeups/tree/master/bside2021/shared_knote/work) from Bsides Ahmedabad 2021
  - no-SMEP/ no-SMAP/ KPTI/ KASLR/ modprobe_path search/ invalid `fpos` use/ zero-addr mapping
  - https://blog.smallkirby.com/posts/shared-knote/
  - I added `check /proc/sys/vm/mmap_min/addr` in my first procedures list.

- [pprofile](https://github.com/smallkirby/pwn-writeups/tree/master/line2021/pprofile/work) from LINE CTF 2021
  - SMEP/ SMAP/ KPTI/ KASLR/ pointer validation/ modprobe_path
  - https://blog.smallkirby.com/posts/pprofile/
  - I realized again that validating userland pointer is important and kernel can endure some wirte-bruteforce. If the validation doesn't exist in distributed binaries, I should recall this chall.
- [nutty](https://github.com/smallkirby/pwn-writeups/tree/master/union2021/nutty/work) from Union CTF 2021
  - SMEP/ SMAP/ KPTI/ KASLR/ race without uffd/ SLOB
  - https://blog.smallkirby.com/posts/nutty/
  - NOTE: I ignored the heap-overflow bug, and invoked race condition w/o uffd to abuse `tty_struct`. Then, bypass SMAP by kROP in kheap(on `tty_strut`).
- [Day One](https://github.com/smallkirby/pwn-writeups/tree/master/eof2020/dayone/work) from AIS3 EOF CTF 2021 Final
  - SMEP/ SMAP/ KPTI/ KASLR/ eBPF verifier
  - https://blog.smallkirby.com/posts/dayone/
  - NOTE: the first half part of the chall is a little bit boring cuz it is exactly the same with the report of [ZDI](https://www.thezdi.com/blog/2021/1/18/zdi-20-1440-an-incorrect-calculation-bug-in-the-linux-kernel-ebpf-verifier). But, the last half part, where we get root without `bpf_map.btf`, is cool. 
- [kernel-rop](https://github.com/smallkirby/pwn-writeups/tree/master/hxp2020/kernel-rop/work) from hxp CTF 2020
  - SMEP/ SMAP/ KPTI/ FGKASLR/ single-core/ kROP
  - https://blog.smallkirby.com/posts/kernel-rop/
- [SharedHouse](https://github.com/smallkirby/pwn-writeups/tree/master/asis2020quals/shared_house) from ASIS 2020 Quals
  - SMEP/ no-SMAP/ no-KPTI/ single-core/ heap
  - https://blog.smallkirby.com/posts/sharedhouse/
- [Spark](https://github.com/smallkirby/pwn-writeups/tree/master/hitcon2020/spark) from HITCON 2020
  - SMEP/ no-SMAP/ no-KPTI/ signle-core/ heap
  - https://blog.smallkirby.com/posts/spark/
- [hashbrown](https://github.com/smallkirby/pwn-writeups/tree/master/dice2020) from DiceCTF 2021
  - SMEP/ SMAP/ KPTI/ FGKASLR/ double-core/ race/ hardened-heap
  - https://blog.smallkirby.com/posts/hashbrown/
- [gnote](https://github.com/smallkirby/pwn-writeups/tree/master/tw2019) from TokyoWesterns CTF 2019
  - race
  - https://blog.smallkirby.com/posts/gnote/
- [eebpf](https://github.com/smallkirby/pwn-writeups/tree/master/tw2020/eebpf) from TokyoWesterns CTF 2020
  - eBPF/ verifier bug
  - https://blog.smallkirby.com/posts/eebpf/
- [Krazynote](https://github.com/smallkirby/pwn-writeups/tree/master/balsn2019/krazynote) from BalsnCTF 2019
  - classic uffd/ race
  - https://blog.smallkirby.com/posts/krazynote/

**Frequently Updated now...**  


## Nirugiri
I don't know these challs are difficult or not and good or soso. But at least I feel these challs are worth solving.

- [hwdbg]() from CakeCTF 2021
  - TODO: writeup
  - SMEP/ SMAP/ KPTI/ KASLR/ physicasl memory access/ modprobe_path
  - https://blog.smallkirby.com/posts/cakectf2021/
  - You have AAW towards `/dev/mem/`, so all the protection is useless. However, `CONFIG_DEVMEM_STRICT` is enabled and you would be confused if you don't know this config.

- [Stonks Socket](https://github.com/smallkirby/pwn-writeups/tree/master/hacklu2021/stonks_socket/work) from Hack.lu CTF 2021
  - no-SMEP/ no-SMAP/ race w/o uffd/ custom TCP hook
  - https://blog.smallkirby.com/posts/stonks-socket/
  - challenge itself is a little bit buggy, but enough for practice of race w/o uffd.


**Frequently Updated now...**  

## Beginners
If you don't know how to prepare for solving kernel-pwn, please refer to [start-kernel-pwning](https://github.com/smallkirby/kernelpwn/tree/master/start-kernel-pwning).

- [lkgit(TBD)](#) from TSGCTF 2021
  - SMEP/SMAP/KPTI/KASLR/race
  - simple uffd, though appropriate fault timing handling is required.
  - https://blog.smallkirby.com/posts/lkgit/
- [meowmeow](https://github.com/smallkirby/pwn-writeups/tree/master/zer0pts2020/meowmeow) from zer0pts CTF 2020
  - buffer overflow
  - https://blog.smallkirby.com/posts/meowmow/
- [kstack](https://github.com/smallkirby/pwn-writeups/tree/master/seccon2020/kstack) from SECCON CTF 2020
  - race
  - https://blog.smallkirby.com/posts/kstack/
- [babydriver](https://github.com/smallkirby/pwn-writeups/tree/master/ncstisc2018/babydriver) from NCSTISC CTF 2018
  - kUAF
  - https://blog.smallkirby.com/posts/babydriver/
- [flitbip](https://github.com/smallkirby/pwn-writeups/tree/master/midnightsun2018/flitbip) from Midnightsun CTF 2018 Finals
  - no-SMEP/ no-SMAP/ no-KASLR/ custom syscall
  - https://blog.smallkirby.com/posts/flitbip/

**Frequently Updated now...**

## Techniques
I want to know some techniques to pwn in kernelland. If you know something I should know, please tell me.
For the techniques I used to solve the challs listed above is listed [HERE(under construction)](./technique).

**Frequently Updated now...**  


## Configs to check
Kernel is distributed in the form of `bzImage` and no information about build configuration is not provided in 99% cases. However, you have to change the way of exploit depending on the configuration. 
Some config might hide important information. Some config might randomize the memory layout. Some config might make variables only readable...
[`important_config`](important_config/README.md) directory collects the important configurations and tries to summarize how to check if it is enabled and how to bypass it.


## My Blog
My blog posts contain not only kernel-writeup, but normal userland-pwn and technique frequently used in pwning and so on...
Please [check it out](https://blog.smallkirby.com/).
**NOTE**:
The most parts of my blog is written when I'm solving the chall as a memo in HackMD and is converted to blog entry with [md2html converter](https://github.com/smallkirby/hackmd2hatena) . I received some DMs which ask me to write my blog posts in Englinsh and I'm willing to, cuz writing in English is not a so heavy task for me (regardless of the quality of my English). I know that my blog is ill-translated by Google translater :(    I'm planning to write them in English someday in the near future.


## References

List of usefull resources are listed under [reference directory](./reference).  

If you have any suggestions, feel free to contact me on [Twitter](https://twitter.com/smallkirby).

  
  
## LICENSE

This repository is licensed under [MIT](./LICENSE).

Note that this license is applied only to WHAT I WROTE.

Binary of kernel challenges themselves would be in many cases licensed under other licenses.

Please follow these in that case.

If you think this or related repositories violate your rights, please contact me.
