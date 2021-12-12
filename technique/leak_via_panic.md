# leak via panic message

# Pre-requisites

- NOKASLR (or full knowledge of memory)
- the kernel uses initramfs
- full control of RIP
- (goal is to read `flag`)

# Exploit

```.asm
jmp &flag
```

It invokes kernel panic, and shows content of `flag` in `CODE` section.


# Refs

- `kone_gadget` from `SECCON CTF2021`.
  - intendend solution itself is cool, but this exploit method is simpler and more surprising...
