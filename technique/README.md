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
  - get RIP back to userland process while in kernel path by `userfaultfd`, which makes race-condition really stable. For the detailed principle, check [HERE](./userfualtfd.md).
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
  - **NOTE** Free-pointers in SLUB objects are now placed in the middle of objects (word aligned). This is introduced in [this patch(3202fa62fb43087387c65bfa9c100feffac74aa6)](https://lkml.org/lkml/2020/3/5/1129). Hence, overwriting the pointers is now difficult. It is introduced in April 1st, but it isn't fool at all... :(
```c
	} else if (freepointer_area > sizeof(void *)) {
		/*
		 * Store freelist pointer near middle of object to keep
		 * it away from the edges of the object to avoid small
		 * sized over/underflows from neighboring allocations.
		 */
		s->offset = ALIGN(freepointer_area / 2, sizeof(void *));
	}
```

# get RIP / into LPE
## the most classic LPE
  - TODO
  - example challs: TODO
## use modprobe_path
  - overwrite `modoprobe_path` with your evil shellscript's path. Then execve a binary which has an unknown format. For detailed principle, check [HERE](./modprobe_path.md).
  - **You can/might use this technique WITHOUT ANY LEAK**. `modprobe_path` has one-byte entropy under KASLR. You can bruteforce the one byte if you have AAW.
    - example chall: [pprofile](https://github.com/smallkirby/pwn-writeups/tree/master/line2021/pprofile/work) from LINE CTF 2021
  - example challs: TODO
## get RIP by n_tty_ops (w/ AAW)
  - TODO
  - example challs: TODO
## get RIP by cleanup of subprocess_info
  - TODO
  - example challs: TODO

## abuse vDSO
  - TODO

## bypass SMAP via kROP in kheap
  - TODO

## bypass SMAP/SMEP by directly change CR4
  - TODO

## temporaly bypass KPTI by setting CR3
  - TODO

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