# userfaultfd
This MD describes how well-known uffd technique works.  

## syscall
`userfaultfd` syscall handler is defined in `fs/userfaultfd.c`. The wrapper of this syscall is not defined in glibc, so you have to call `syscall` directly.
```c fs/userfaultfd.c
SYSCALL_DEFINE1(userfaultfd, int, flags)
```

It just alloc a cache for `struct userfaultfd_ctx` from `userfaultfd_ctx_cachep`. Then it creates annonymous inode named `[userfaultfd]` and returns fd of it to userland. `userfaultfd_ctx` is stored in `private_data` of the file. For `file_operations` of the file, `userfaultfd_fops` is assigned to.
```c fs/userfualtfd.c
static const struct file_operations userfaultfd_fops = {
#ifdef CONFIG_PROC_FS
	.show_fdinfo	= userfaultfd_show_fdinfo,
#endif
	.release	= userfaultfd_release,
	.poll		= userfaultfd_poll,
	.read		= userfaultfd_read,
	.unlocked_ioctl = userfaultfd_ioctl,
	.compat_ioctl	= compat_ptr_ioctl,
	.llseek		= noop_llseek,
};
```

## just move forward the state
After we call `userfaultfd` syscall, `userfaultfd_ctx.state` is set to `UFFD_STATE_WAIT_API`.  To make forward the state into `UFFD_STATE_RUNNING`, we have to `ioctl(UFFDIO_API)` and ask the kernel that which features are enabled for uffd API. The answer is returned from kernel, but we don't use it in exploits.
```c
  // enable uffd object via ioctl(UFFDIO_API)
  uffdio_api.api = UFFD_API;
  uffdio_api.features = 0;
  if(ioctl(uffd, UFFDIO_API, &uffdio_api) == -1)
    errExit("ioctl-UFFDIO_API");
```
  
## register
To register memory region to be supervised, we call `ioctl(UFFDIO_REGISTER)`. 
```c
  // specify memory region handled by userfaultfd via ioctl(UFFDIO_REGISTER)
  uffdio_register.range.start = addr;
  uffdio_register.range.len = len;
  uffdio_register.mode = UFFDIO_REGISTER_MODE_MISSING;
  if(ioctl(uffd, UFFDIO_REGISTER, &uffdio_register) == -1)
    errExit("ioctl-UFFDIO_REGISTER");
```

This call is handled in `userfaultfd_register()@fs/userfaultfd.c`.  

- Check `range` to be supervised.
  - `start` addr must be page-aligned.
  - `len` must be page-aligned.
  - `start` must be larger than `mmap_min_addr`, which is `1K` by default.
  - `start` must be smaller than `task_size`.
- Check VMAs contained in specified range can be supervised.
  - Check vma is not anonymous or `vma->vmops` is non-NULL.
  - Check vma has `VM_MAYWRITE` flag.
  - Check vma is not supervised by other uffds.
- Register uffd in *vma*s.
  - set `VM_UFFD_MISSING` bit into `vma->vma_flags`.
  - set `ctx` in `vma->vm_userfaultfd_ctx.ctx`.
  - If the range contains multiple vma regions or ends in the half of a vma region, split or merge *vma*s.

Kernel fills `uffdio_register.ioctls` with ioctls which would suceed for the registered uffd, but we don't use the info in exploits.

## handle page fault
The handler of page fault is registered as below.
```c
SYM_CODE_START(irq_entries_start)
    vector=FIRST_EXTERNAL_VECTOR
    .rept (FIRST_SYSTEM_VECTOR - FIRST_EXTERNAL_VECTOR)
	UNWIND_HINT_IRET_REGS
0 :
	.byte	0x6a, vector
	jmp	asm_common_interrupt
	nop
	/* Ensure that the above is 8 bytes max */
	. = 0b + 8
	vector = vector+1
    .endr
SYM_CODE_END(irq_entries_start)

(snipped...)
DECLARE_IDTENTRY_RAW_ERRORCODE(X86_TRAP_PF,	exc_page_fault);
```

It calls `handle_page_fault()`. It checks whether the fault is in userland or kernelland. If it's in userland, it calls `do_user_addr_fault()`. FYI, SMAP is handled also here.
```c
	if (unlikely(cpu_feature_enabled(X86_FEATURE_SMAP) &&
		     !(hw_error_code & X86_PF_USER) &&
		     !(regs->flags & X86_EFLAGS_AC)))
	{
		bad_area_nosemaphore(regs, hw_error_code, address);
		return;
	}
```

Anyway, `mmap()` just creates new vma, and doesn't setup PTE (you have to specify `MAP_PRIVATE` for `mmap`). In this case, `handle_user_addr_fault()` calls `handle_mm_fault()`, `__handle_mm_fault()`, `handle_pte_fault()`, then `do_anonymous_page()` cuz PTE is not initialized at all in `mmap()`. In this func, it reaches below path cuz we set `VM_UFFD_MISSING` bit of `vma->vm_flags` before.  
```c
	if (userfaultfd_missing(vma)) {
		pte_unmap_unlock(vmf->pte, vmf->ptl);
		put_page(page);
		return handle_userfault(vmf, VM_UFFD_MISSING);
	}
```

Here, this event is pushed into uffd wait queue of the ctx.
```c
	init_waitqueue_func_entry(&uwq.wq, userfaultfd_wake_function);
	uwq.wq.private = current;
	uwq.msg = userfault_msg(vmf->address, vmf->flags, reason,
			ctx->features);
	uwq.ctx = ctx;
	uwq.waken = false;
(snipped...)
	__add_wait_queue(&ctx->fault_pending_wqh, &uwq.wq);
```
Then, it waits until userland program polls the event.
```c
	if (likely(must_wait && !READ_ONCE(ctx->released))) {
		wake_up_poll(&ctx->fd_wqh, EPOLLIN);
		schedule();
	}
```

## do your business in userland
Now, the RIP is back to the userland program and you can do what you like. You can catch the fault event by polling for uffd.
```c
  while(poll(&pollfd, 1, -1) > 0){
    if(pollfd.revents & POLLERR || pollfd.revents & POLLHUP)
      errExit("poll");

    // read an event
    if(read(uffd, &msg, sizeof(msg)) == 0)
      errExit("read");

    if(msg.event != UFFD_EVENT_PAGEFAULT)
      errExit("unexpected pagefault");

    printf("[!] page fault: %p\n",msg.arg.pagefault.address);
```

You can copy the content in the faulted-page, or unwatch that page by `ioctl()`.


## Mitigation
`userfaultfd` can be disabled by `CONFIG_USERFAULTFD`. In this case, you can call any of uffd-related functions. If you do, below message would appear.