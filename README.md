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
- [pprofile](https://github.com/smallkirby/pwn-writeups/tree/master/line2021/pprofile/work) from LINE CTF 2021
  - SMEP/ SMAP/ KPTI/ KASLR/ pointer validation/ modprobe_path
  - https://smallkirby.hatenablog.com/entry/2021/03/21/160134
  - I realized again that validating userland pointer is important and kernel can endure some wirte-bruteforce. If the validation doesn't exist in distributed binaries, I should recall this chall.
- [nutty](https://github.com/smallkirby/pwn-writeups/tree/master/union2021/nutty/work) from Union CTF 2021
  - SMEP/ SMAP/ KPTI/ KASLR/ race without uffd/ SLOB
  - https://smallkirby.hatenablog.com/entry/2021/02/22/053507
  - NOTE: I ignored the heap-overflow bug, and invoked race condition w/o uffd to abuse `tty_struct`. Then, bypass SMAP by kROP in kheap(on `tty_strut`).
- [Day One](https://github.com/smallkirby/pwn-writeups/tree/master/eof2020/dayone/work) from AIS3 EOF CTF 2021 Final
  - SMEP/ SMAP/ KPTI/ KASLR/ eBPF verifier
  - https://smallkirby.hatenablog.com/entry/2021/02/20/131428
  - NOTE: the first half part of the chall is a little bit boring cuz it is exactly the same with the report of [ZDI](https://www.thezdi.com/blog/2021/1/18/zdi-20-1440-an-incorrect-calculation-bug-in-the-linux-kernel-ebpf-verifier). But, the last half part, where we get root without `bpf_map.btf`, is cool. 
- [kernel-rop](https://github.com/smallkirby/pwn-writeups/tree/master/hxp2020/kernel-rop/work) from hxp CTF 2020
  - SMEP/ SMAP/ KPTI/ FGKASLR/ single-core/ kROP
  - https://smallkirby.hatenablog.com/entry/2021/02/16/225125
- [SharedHouse](https://github.com/smallkirby/pwn-writeups/tree/master/asis2020quals/shared_house) from ASIS 2020 Quals
  - SMEP/ no-SMAP/ no-KPTI/ single-core/ heap
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

**Frequently Updated now...**  


## Beginners
If you don't know how to prepare for solving kernel-pwn, please refer to [start-kernel-pwning](https://github.com/smallkirby/kernelpwn/tree/master/start-kernel-pwning).

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

**Frequently Updated now...**

## Techniques
I want to know some techniques to pwn in kernelland. If you know something I should know, please tell me.
For the techniques I used to solve the challs listed above is listed [HERE(under construction)](./technique).


## My Blog
My blog posts contain not only kernel-writeup, but normal userland-pwn and technique frequently used in pwning and so on...
Please [check it out](https://smallkirby.hatenablog.com/archive).
**NOTE**:
The most parts of my blog is written when I'm solving the chall as a memo in HackMD and is converted to blog entry with [md2html converter](https://github.com/smallkirby/hackmd2hatena) . I received some DMs which ask me to write my blog posts in Englinsh and I'm willing to, cuz writing in English is not a so heavy task for me (regardless of the quality of my English). I know that my blog is ill-translated by Google translater :(    I'm planning to write them in English someday in the near future.

## References
List of usefull resources for general.
- [Usefull Structures in kernel by Master of pwning](https://ptr-yudai.hatenablog.com/entry/2020/03/16/165628)
- [Kernel pwn challs collection by BrieflyX](https://github.com/BrieflyX/ctf-pwns)
- [Non-userland pwn challs collection by hama](https://hama.hatenadiary.jp/entry/2019/12/01/231213)


If you have any suggestions, feel free to contact me on [Twitter](https://twitter.com/smallkirby_).
  
  
#### DISCLAIMER
You can cite or use my code and comments in this repo as you like basically.  
The exceptions are when the code belongs to some other licenses. In that case, follow them. Also, you can't use them for evil purpose. Finally, I don't take any responsibility for using my code or comment.  
If you find something useful, or you notice that hoge is wrong, I'll appreciate if you leave comments via PR, Issue, DM, or Blog.