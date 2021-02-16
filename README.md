# kernel pwn
## About this repo
This repository collects CTF kernel-pwn challenges and writeups.
Also, it introduces how to start learning kernel-pwn for beginners **including me**.
All the challs here are solved by me, though the writeup may be based on the author's one or others's ones.
I'm planning to include not only kernel-pwn, but also general non-userland pwn including QEMU, V8, multi-arch...

**IMPORTANT**:
**If you know some good kernel(non-userland) pwn challs, please tell me and I'll solve it. Then I'll add it on this repo if I feel it good to solve.**


## Good Challs
- [SharedHouse](https://github.com/smallkirby/pwn-writeups/tree/master/asis2020quals/shared_house) from ASIS 2020 Quals
  - SMEP/ SMAP/ no-KPTI/ single-core/ heap
  - https://smallkirby.hatenablog.com/entry/2021/02/13/230520
- [Spark](https://github.com/smallkirby/pwn-writeups/tree/master/hitcon2020/spark) from HITCON 2020
  - SMEP/ no-SMAP/ no-KPTI/ signle-core/ heap
  - https://smallkirby.hatenablog.com/entry/2021/02/09/133655
- [hashbrown](https://github.com/smallkirby/pwn-writeups/tree/master/dice2020) from DiceCTF 2021
  - SMEP/ SMAP/ KPTI/ FGKASLR/ double-core/ race/ hardened-heap
  - https://smallkirby.hatenablog.com/entry/2021/02/15/215158
- [gnote](https://github.com/smallkirby/pwn-writeups/tree/master/tw2019) from TokyoWesterns CTF 2019
  - race
  - https://smallkirby.hatenablog.com/entry/2019/11/19/225504
- [eebpf](https://github.com/smallkirby/pwn-writeups/tree/master/tw2020/eebpf) from TokyoWesterns CTF 2020
  - eBPF/ verifier bug
  - https://smallkirby.hatenablog.com/entry/2021/01/31/210158
- [Krazynote](https://github.com/smallkirby/pwn-writeups/tree/master/balsn2019/krazynote) from BalsnCTF 2019
  - classic uffd/ race
  - https://smallkirby.hatenablog.com/entry/2020/08/09/085028

Frequently Updated now...


## Beginners
If you don't know how to prepare for solving kernel-pwn, please refer to [start-kernel-pwning](https://github.com/smallkirby/kernelpwn/start-kernel-pwning).

- [meowmeow](https://github.com/smallkirby/pwn-writeups/tree/master/zer0pts2020/meowmeow) from zer0pts CTF 2020
  - buffer overflow
  - https://smallkirby.hatenablog.com/entry/2020/07/23/221605
- [kstack](https://github.com/smallkirby/pwn-writeups/tree/master/seccon2020/kstack) from SECCON CTF 2020
  - race
  - https://smallkirby.hatenablog.com/entry/2020/10/14/184245
- [babydriver](https://github.com/smallkirby/pwn-writeups/tree/master/ncstisc2018/babydriver) from NCSTISC CTF 2018
  - kUAF
  - https://smallkirby.hatenablog.com/entry/2021/02/14/110450
- [flitbip](https://github.com/smallkirby/pwn-writeups/tree/master/midnightsun2018/flitbip) from Midnightsun CTF 2018 Finals
  - no-SMEP/ no-SMAP/ no-KASLR/ custom syscall
  - https://smallkirby.hatenablog.com/entry/2021/02/14/142626

Frequently Updated now...


## My Blog
My blog posts contain not only kernel-writeup, but normal userland-pwn and technique frequently used in pwning and so on...
Please [check it out](https://smallkirby.hatenablog.com/archive).

## References
List of usefull resources for general.
- [Usefull Structures in kernel by Master of pwning](https://ptr-yudai.hatenablog.com/entry/2020/03/16/165628)
- [Kernel pwn challs collection by BrieflyX](https://github.com/BrieflyX/ctf-pwns)
- [Non-userland pwn challs collection by hama](https://hama.hatenadiary.jp/entry/2019/12/01/231213)


If you have any suggestions, feel free to contact me on [Twitter](https://twitter.com/smallkirby_).
