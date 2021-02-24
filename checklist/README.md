# Checklist
This dir contains a checklist when solving kernel-pwn challs. The content contains the list of what you can do under the specific situation (e.g. when SMAP/SMEP/FGKASLR are enabled, when you have AAR, when you have AAW but SLUB is broken, etc...) and the list of you have to check when you somehow can't pwn the kernel remotely.  **You might feel them really something petty**, but it is important for me cuz I really hate the situation where one small typo in my exploit ruins the whole thing, then I notice it after the CTF is over...  
In addition, this dir serves as a reference to [technique dir](../technique/), which explains when, how, why you can use the technique.

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