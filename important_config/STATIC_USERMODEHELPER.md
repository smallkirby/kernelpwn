# CONFIG_STATIC_USERMODEHELPER

## what's it

- If this config is set, `call_usermodehelper_setup()` uses constant value as a helper binary name.
- If not set, the function uses 1st argument as a helper binary.

## what's effect
- Well-known [`modprobe_path` overwriting technique](../technique/modprobe_path.md) overwrites this variable to run arbitrary evil script as root. 
- If this config is set, it is useless to overwrite `modprobe_path` anymore cuz the variable is not used.
  - Note that even if the config is set, `modprobe_path` exists and can be overwritten.

## Principle / Code
This function is called from `call_modprobe()` at `/kernel/kmod.c`.  In the same file, `modprobe_path` is defined.
```kmod.c
char modprobe_path[KMOD_PATH_LEN] = CONFIG_MODPROBE_PATH;

static int call_modprobe(char *module_name, int wait)
{
	struct subprocess_info *info;
  ...
	argv[0] = modprobe_path;
  ...

	info = call_usermodehelper_setup(modprobe_path, argv, envp, GFP_KERNEL,
					 NULL, free_modprobe_argv, NULL);
  ...
	return call_usermodehelper_exec(info, wait | UMH_KILLABLE);
  ...
}
```

The functions calls `call_usermodehelper_setup()` to make a `struct subprocess_info` of helper binary. Here, `path` is set as 1st argument if `STATIC_USERMODEHELPER` is not set. If set, this function ignore the 1st argument and uses `CONFIG_USERMODEHELPER_PATH`, which is specified by the user on build-time.
```umh.c
struct subprocess_info *call_usermodehelper_setup(const char *path, char **argv,
		char **envp, gfp_t gfp_mask,
		int (*init)(struct subprocess_info *info, struct cred *new),
		void (*cleanup)(struct subprocess_info *info),
		void *data)
{
	struct subprocess_info *sub_info;
	sub_info = kzalloc(sizeof(struct subprocess_info), gfp_mask);
  ...

#ifdef CONFIG_STATIC_USERMODEHELPER
	sub_info->path = CONFIG_STATIC_USERMODEHELPER_PATH;
#else
	sub_info->path = path;
#endif
  ...
	return sub_info;
}
EXPORT_SYMBOL(call_usermodehelper_setup);
```

## how to check if enabled

1. Check the addr of `call_usermodehelper_setup()` from `/proc/kallsyms`.
```1.sh
/ # cat /proc/kallsyms | grep call_usermodehelper_setup
ffffffff810c8c80 T call_usermodehelper_setup
ffffffff82458118 r __ksymtab_call_usermodehelper_setup
ffffffff82477e3d r __kstrtab_call_usermodehelper_setup
ffffffff8247c3c4 r __kstrtabns_call_usermodehelper_setup
```

2. Go to GDB and set a break at `call_usermodehelper_setup()`.
```2.sh
hb *0xffffffff810c8c80
```

3. Invoke it.
```3.sh
echo -ne "\xff\xff\xff\xff" > /tmp/nirugiri
chmod +x /tmp/nirugiri
/tmp/nirugiri
```

4. Go to GDB and disass the function.
  - If the config is **NOT set**, the function looks like below:
```not_set.S
Breakpoint 1, call_usermodehelper_setup (path=path@entry=0xffffffff8265ff00 <modprobe_path> "/sbin/modprobe", argv=argv@entry=0xffff8880024c3200, envp=envp@entry=0xffffffff8265fe80 <envp>,
    gfp_mask=gfp_mask@entry=3264, init=init@entry=0x0, cleanup=cleanup@entry=0xffffffff810d6d00 <free_modprobe_argv>, data=0x0) at kernel/umh.c:361
361     {
(gdb) disass
Dump of assembler code for function call_usermodehelper_setup:
=> 0xffffffff810c8c80 <+0>:     data16 data16 data16 xchg ax,ax
   0xffffffff810c8c85 <+5>:     mov    eax,ecx
   0xffffffff810c8c87 <+7>:     push   r14
   0xffffffff810c8c89 <+9>:     mov    r14,rdi
   0xffffffff810c8c8c <+12>:    push   r13
   0xffffffff810c8c8e <+14>:    and    eax,0x1
   0xffffffff810c8c91 <+17>:    mov    r13,rsi
   0xffffffff810c8c94 <+20>:    mov    esi,ecx
   0xffffffff810c8c96 <+22>:    add    eax,0x1
   0xffffffff810c8c99 <+25>:    or     esi,0x100
   0xffffffff810c8c9f <+31>:    and    ecx,0x11
   0xffffffff810c8ca2 <+34>:    push   r12
   0xffffffff810c8ca4 <+36>:    cmovne ecx,eax
   0xffffffff810c8ca7 <+39>:    push   rbp
   0xffffffff810c8ca8 <+40>:    mov    r12,rdx
   0xffffffff810c8cab <+43>:    mov    edx,0x60
   0xffffffff810c8cb0 <+48>:    push   rbx
   0xffffffff810c8cb1 <+49>:    mov    rbp,r9
   0xffffffff810c8cb4 <+52>:    mov    rbx,r8
   0xffffffff810c8cb7 <+55>:    mov    ecx,ecx
   0xffffffff810c8cb9 <+57>:    lea    rax,[rcx*8+0x0]
   0xffffffff810c8cc1 <+65>:    sub    rax,rcx
   0xffffffff810c8cc4 <+68>:    shl    rax,0x4
   0xffffffff810c8cc8 <+72>:    mov    rdi,QWORD PTR [rax-0x7dbd0318]
   0xffffffff810c8ccf <+79>:    call   0xffffffff812271c0 <kmem_cache_alloc_trace>
   0xffffffff810c8cd4 <+84>:    test   rax,rax
   0xffffffff810c8cd7 <+87>:    je     0xffffffff810c8d17 <call_usermodehelper_setup+151>
   0xffffffff810c8cd9 <+89>:    lea    rdx,[rax+0x8]
   0xffffffff810c8cdd <+93>:    mov    QWORD PTR [rax+0x18],0xffffffff810c89e0
   0xffffffff810c8ce5 <+101>:   movabs rdi,0xfffffffe0
   0xffffffff810c8cef <+111>:   mov    QWORD PTR [rax+0x8],rdx
   0xffffffff810c8cf3 <+115>:   mov    QWORD PTR [rax+0x10],rdx
   0xffffffff810c8cf7 <+119>:   mov    rdx,QWORD PTR [rsp+0x30]
   0xffffffff810c8cfc <+124>:   mov    QWORD PTR [rax],rdi
   0xffffffff810c8cff <+127>:   mov    QWORD PTR [rax+0x28],r14
   0xffffffff810c8d03 <+131>:   mov    QWORD PTR [rax+0x30],r13
   0xffffffff810c8d07 <+135>:   mov    QWORD PTR [rax+0x38],r12
   0xffffffff810c8d0b <+139>:   mov    QWORD PTR [rax+0x50],rbp
   0xffffffff810c8d0f <+143>:   mov    QWORD PTR [rax+0x48],rbx
   0xffffffff810c8d13 <+147>:   mov    QWORD PTR [rax+0x58],rdx
   0xffffffff810c8d17 <+151>:   pop    rbx
   0xffffffff810c8d18 <+152>:   pop    rbp
   0xffffffff810c8d19 <+153>:   pop    r12
   0xffffffff810c8d1b <+155>:   pop    r13
   0xffffffff810c8d1d <+157>:   pop    r14
   0xffffffff810c8d1f <+159>:   ret
End of assembler dump.
```

  - If the config is **set**, it looks like below:
```set.S
Breakpoint 1, call_usermodehelper_setup (path=path@entry=0xffffffff8265ff00 <modprobe_path> "/sbin/modprobe", argv=argv@entry=0xffff8880024c2380, envp=envp@entry=0xffffffff8265fe80 <envp>,
    gfp_mask=gfp_mask@entry=3264, init=init@entry=0x0, cleanup=cleanup@entry=0xffffffff810d6d00 <free_modprobe_argv>, data=0x0) at kernel/umh.c:361
361     {
(gdb) disass
Dump of assembler code for function call_usermodehelper_setup:
=> 0xffffffff810c8c80 <+0>:     data16 data16 data16 xchg ax,ax
   0xffffffff810c8c85 <+5>:     mov    eax,ecx
   0xffffffff810c8c87 <+7>:     push   r13
   0xffffffff810c8c89 <+9>:     mov    r13,rsi
   0xffffffff810c8c8c <+12>:    mov    esi,ecx
   0xffffffff810c8c8e <+14>:    and    eax,0x1
   0xffffffff810c8c91 <+17>:    or     esi,0x100
   0xffffffff810c8c97 <+23>:    push   r12
   0xffffffff810c8c99 <+25>:    mov    r12,rdx
   0xffffffff810c8c9c <+28>:    add    eax,0x1
   0xffffffff810c8c9f <+31>:    and    ecx,0x11
   0xffffffff810c8ca2 <+34>:    push   rbp
   0xffffffff810c8ca3 <+35>:    mov    edx,0x60
   0xffffffff810c8ca8 <+40>:    cmovne ecx,eax
   0xffffffff810c8cab <+43>:    push   rbx
   0xffffffff810c8cac <+44>:    mov    rbp,r9
   0xffffffff810c8caf <+47>:    mov    rbx,r8
   0xffffffff810c8cb2 <+50>:    mov    ecx,ecx
   0xffffffff810c8cb4 <+52>:    lea    rax,[rcx*8+0x0]
   0xffffffff810c8cbc <+60>:    sub    rax,rcx
   0xffffffff810c8cbf <+63>:    shl    rax,0x4
   0xffffffff810c8cc3 <+67>:    mov    rdi,QWORD PTR [rax-0x7dbd0318]
   0xffffffff810c8cca <+74>:    call   0xffffffff812271c0 <kmem_cache_alloc_trace>
   0xffffffff810c8ccf <+79>:    test   rax,rax
   0xffffffff810c8cd2 <+82>:    je     0xffffffff810c8d16 <call_usermodehelper_setup+150>
   0xffffffff810c8cd4 <+84>:    lea    rdx,[rax+0x8]
   0xffffffff810c8cd8 <+88>:    mov    QWORD PTR [rax+0x18],0xffffffff810c89e0
   0xffffffff810c8ce0 <+96>:    movabs rdi,0xfffffffe0
   0xffffffff810c8cea <+106>:   mov    QWORD PTR [rax+0x8],rdx
   0xffffffff810c8cee <+110>:   mov    QWORD PTR [rax+0x10],rdx
   0xffffffff810c8cf2 <+114>:   mov    rdx,QWORD PTR [rsp+0x28]
   0xffffffff810c8cf7 <+119>:   mov    QWORD PTR [rax],rdi
   0xffffffff810c8cfa <+122>:   mov    QWORD PTR [rax+0x28],0xffffffff82351060
   0xffffffff810c8d02 <+130>:   mov    QWORD PTR [rax+0x30],r13
   0xffffffff810c8d06 <+134>:   mov    QWORD PTR [rax+0x38],r12
   0xffffffff810c8d0a <+138>:   mov    QWORD PTR [rax+0x50],rbp
   0xffffffff810c8d0e <+142>:   mov    QWORD PTR [rax+0x48],rbx
   0xffffffff810c8d12 <+146>:   mov    QWORD PTR [rax+0x58],rdx
   0xffffffff810c8d16 <+150>:   pop    rbx
   0xffffffff810c8d17 <+151>:   pop    rbp
   0xffffffff810c8d18 <+152>:   pop    r12
   0xffffffff810c8d1a <+154>:   pop    r13
   0xffffffff810c8d1c <+156>:   ret
```

  - You have to focus on how `$rdi` is used. The important difference is at `<+67>` of the latter. It `mov` a value **without** saving original value of `$rdi`. It ignore the 1st argument. You can easily know that `STATIC_USERMODEHELPER` is set.
  - In contrast, 1st argument is saved into `$r14` at `<+9>` and assigned into `sub_info` at `<+127>` in the former case.
