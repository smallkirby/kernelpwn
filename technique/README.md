# Technique of kernel pwning
This dir lists technique used in kernel pwn.
**IMPORTANT**:
I'm just learning kernel-pwning now and this list might have some wrong description or insufficient points. If you find something wrong or you know some technique I should know, please tell and scold me.

**THIS SECTION IS FREQUENTLY UPDATED NOW.**


# FGKASLR bypass
## use .bss/.data section pointer or non-rondomized functions
  - TODO
  - example challs: TODO
## leak rondomized pointer via __ksymtab_xxx
  - TODO
  - example challs: TODO

# Race Condition
## classic userfaultfd
  - TODO
  - example challs: TODO
## uffd with split object over two pages
  - TODO
  - example challs: TODO

# Heap (SLUB)
## classic heap-spray
  - TODO
  - example challs: TODO
## slub freelist overwrite
  - TODO
  - example challs: TODO

# get RIP / into LPE
## the most classic LPE
  - TODO
  - example challs: TODO
## use modprobe_path
  - TODO
  - example challs: TODO
## get RIP by n_tty_ops (w/ AAW)
  - TODO
  - example challs: TODO
## get RIP by cleanup of subprocess_info
  - TODO
  - example challs: TODO

# kROP
## init_task traversal to find current_cred
  - TODO
  - example challs: TODO
## mark comm via prctl to find current_cred
  - TODO
  - example challs: TODO
## return to userspace safely (under SMAP/SMEP)
  - TODO
  - example challs: TODO

# Shellcode (without SMEP)
  - TODO
  - example challs: TODO

# eBPF
  - TODO
  - example challs: TODO