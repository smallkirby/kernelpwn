# modprobe_path
This MD describes how well-known modprobe_path technique works.  

## Abstraction of modprobe_path technique
`modprobe_path` overwriting technique is well-known technique to run your arbitrary shellscript as root privilege. This technique requires below conditions.

1. You have AAW, to overwrite `modprobe_path`.
2. You can write arbitrary two files. One has mal-formed format such as `\xff\xff\xff\xff`. The other is a shellscript which do what you like as root.
3. You can execve the latter file of 2.
4. `CONFIG_STATIC_USERMODEHELPER` is not enabled.

In the most cases, 2 and 3 are fulfilled in kernel-pwn challs. So virtually, the only condition is that you have AAW.  
  

The sections below describes the principle of this tech.

## determine binary format
When you type `./hoge` on your shell, it calls `execve` systemcall.
```c fs/exec.c
SYSCALL_DEFINE3(execve,
		const char __user *, filename,
		const char __user *const __user *, argv,
		const char __user *const __user *, envp)
{
	return do_execve(getname(filename), argv, envp);
}
```

This calls `do_execveat_common()` > `bprm_execve()` > `exec_binprm()`, then `search_binary_handler()`. This func searches `formats` for the appropriate loader function for the specified binary. `formats` is just a list of `struct linux_binfmt`.
```sh formats.sh
pwndbg> delist formats
$22 = {
  next = 0xffffffff8248d6e0 <script_format>,
  prev = 0xffffffff824861c0 <formats>
}
$23 = {
  next = 0xffffffff8248d720 <elf_format>,
  prev = 0xffffffff8248d640 <misc_format>
}
$24 = {
  next = 0xffffffff8248d760 <compat_elf_format>,
  prev = 0xffffffff8248d6e0 <script_format>
}
$25 = {
  next = 0xffffffff824861c0 <formats>,
  prev = 0xffffffff8248d720 <elf_format>
}
$26 = {
  next = 0xffffffff8248d640 <misc_format>,
  prev = 0xffffffff8248d760 <compat_elf_format>
}
```

This list has four formats. `elf_format` is a common format of ELF binary. `compat_elf_format` seems to be identical with `elf_format`. `script_format` is for script files which begin with shebang(`#!`). `misc_format` is for misc binaries. I haven't checked it deeply, but binary types for misc can be registered separetely. Each formats have below structure. The most important member here is `load_binary`.
```sh formats-der.sh
pwndbg> p script_format
$27 = {
  lh = {
    next = 0xffffffff8248d720 <elf_format>,
    prev = 0xffffffff8248d640 <misc_format>
  },
  module = 0x0,
  load_binary = 0xffffffff81222e90 <load_script>,
  load_shlib = 0x0,
  core_dump = 0x0,
  min_coredump = 0
}
pwndbg> p elf_format
$28 = {
  lh = {
    next = 0xffffffff8248d760 <compat_elf_format>,
    prev = 0xffffffff8248d6e0 <script_format>
  },
  module = 0x0,
  load_binary = 0xffffffff812236c0 <load_elf_binary>,
  load_shlib = 0xffffffff81223210 <load_elf_library>,
  core_dump = 0xffffffff81224d30 <elf_core_dump>,
  min_coredump = 4096
}
pwndbg> p compat_elf_format
$29 = {
  lh = {
    next = 0xffffffff824861c0 <formats>,
    prev = 0xffffffff8248d720 <elf_format>
  },
  module = 0x0,
  load_binary = 0xffffffff812265b0 <load_elf_binary>,
  load_shlib = 0xffffffff81226110 <load_elf_library>,
  core_dump = 0xffffffff81227c60 <elf_core_dump>,
  min_coredump = 4096
}
pwndbg> p misc_format
$30 = {
  lh = {
    next = 0xffffffff8248d6e0 <script_format>,
    prev = 0xffffffff824861c0 <formats>
  },
  module = 0x0,
  load_binary = 0xffffffff812223e0 <load_misc_binary>,
  load_shlib = 0x0,
  core_dump = 0x0,
  min_coredump = 0
}
```

In `search_binary_handler()`, `formats` list is for-looped and each `load_binary()` is called for the binary.
```c fs/exec.c
 retry:
	read_lock(&binfmt_lock);
	list_for_each_entry(fmt, &formats, lh) {
		if (!try_module_get(fmt->module))
			continue;
		read_unlock(&binfmt_lock);

		retval = fmt->load_binary(bprm);

		read_lock(&binfmt_lock);
		put_binfmt(fmt);
		if (bprm->point_of_no_return || (retval != -ENOEXEC)) {
			read_unlock(&binfmt_lock);
			return retval;
		}
	}
	read_unlock(&binfmt_lock);
```

For example, `load_binary` of `elf_format` is `load_elf_binary()`. It just checks the magic(`\x7FELF`) for ELF header of the binary.
```c fs/binfmt_elf.c
 	if (memcmp(elf_ex->e_ident, ELFMAG, SELFMAG) != 0)
		goto out;
```

`load_binary` of `script_format` is `load_script()`. It just checks shebang. If shebang is vaild, `bprm->interpreter` is set to the specified interpreter and the original binary name becomes the one of argv, then re-search the appropriate handler in `search_binary_handler()`. 
```c fs/binfmt_script.c
	if ((bprm->buf[0] != '#') || (bprm->buf[1] != '!'))
		return -ENOEXEC;
```

If no handler is found for the specified binary format, it reaches the below path in `search_binary_handler()`.
```c fs/exec.c
	if (need_retry) {
		if (printable(bprm->buf[0]) && printable(bprm->buf[1]) &&
		    printable(bprm->buf[2]) && printable(bprm->buf[3]))
			return retval;
		if (request_module("binfmt-%04x", *(ushort *)(bprm->buf + 2)) < 0)
			return retval;
		need_retry = false;
		goto retry;
	}
```

This checks if the specified binary's first 4bytes are not printable(non-ASCIIs w/o tab or newline). Then, it calls `request_module()` with the format name `binfmt-<first 4bytes of the binary>`. It just calls `__request_module()`, then `call_modprobe()`.

## request_module()
```c kernel/kmod.c
static int call_modprobe(char *module_name, int wait)
{
	struct subprocess_info *info;
	static char *envp[] = {
		"HOME=/",
		"TERM=linux",
		"PATH=/sbin:/usr/sbin:/bin:/usr/bin",
		NULL
	};

	char **argv = kmalloc(sizeof(char *[5]), GFP_KERNEL);
	if (!argv)
		goto out;

	module_name = kstrdup(module_name, GFP_KERNEL);
	if (!module_name)
		goto free_argv;

	argv[0] = modprobe_path;
	argv[1] = "-q";
	argv[2] = "--";
	argv[3] = module_name;	/* check free_modprobe_argv() */
	argv[4] = NULL;

	info = call_usermodehelper_setup(modprobe_path, argv, envp, GFP_KERNEL,
					 NULL, free_modprobe_argv, NULL);
	if (!info)
		goto free_module_name;

	return call_usermodehelper_exec(info, wait | UMH_KILLABLE);

free_module_name:
	kfree(module_name);
free_argv:
	kfree(argv);
out:
	return -ENOMEM;
}
```

It tries to load the specified binary as a module. Default helper name is defined as char-array in `kernel/kmod.c` as `/sbin/modprobe`. It is run as root priviledge. And It is not *const*.
```c kernel/kmod.c
char modprobe_path[KMOD_PATH_LEN] = "/sbin/modprobe";
```
So you can overwrite it with your evil shellscript to get shell or to *cat* a flag.

## Mitigation
In `call_usermodehelper_setup()`, `sub_info->path` is set as below.
```c kernel/umh.c
#ifdef CONFIG_STATIC_USERMODEHELPER
	sub_info->path = CONFIG_STATIC_USERMODEHELPER_PATH;
#else
	sub_info->path = path;
#endif
```
Here, `path` is `modprobe_path`. If `CONFIG_STATIC_USERMODEHELPER` is defined, usermode helper is set to `CONFIG_STATIC_USERMODEHELPER_PATH`, which is defined as a constant value on build-time. Therefore, overwriting `modprobe_path` is useless when this config is enabled.  
This patch is introduced int below commit for mitigation of this kind of exploit.
https://lore.kernel.org/lkml/20170810180618.22457-2-mcgrof@kernel.org/