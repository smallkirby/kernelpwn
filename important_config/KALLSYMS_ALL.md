# CONFIG_KALLSYMS_ALL

## what's it

- When you want to know the addr/offset of specific kernel symbol, you would check `/proc/kallsyms`. This pseudo file reveals you addrs of kernel symbols.
- This file is originally for pretty print of OOPS message of kernel. To use `/proc/kallsyms`, kernel config `CONFIG_KALLSYMS` must be set.
- However, this file contains only `.text` symbols (`T` for global functions, `t` for internal functions).  
- To make `/proc/kallsyms` contain all the symbols such as in `.data` section, you must set `CONFIG_KALLSYMS_ALL`.
- `CONFIG_KALLSYMS_ALL` doesn't have default value, so defaults to `N`. In short, you can't know the address of symbols in `.data` section from `/proc/kallsyms`.

## what's effect
- Linux Kernel has some useful symbols such as `core_pattern` and `modprobe_path`.
  - If you have AAW, overwriting these symbols means LPE is achieved.
- However, you can't know the addr/offset of these symbols from `/proc/kallsyms` if `CONFIG_KALLSYMS_ALL` is not set. Then, you have to debug to know the offset. 

## example
- When the config is set to `y`, it looks like below:
```y.txt
/ # cat /proc/kallsyms | grep modprobe
ffffffff810d6d00 t free_modprobe_argv
ffffffff8265ff00 D modprobe_path
```

- When the config is not set and defaults to `n`, it looks like below:
```y.txt
/ # cat /proc/kallsyms | grep modprobe
ffffffff8106f3f0 t free_modprobe_argv
```

## how to bypass
- Debug to know the actuall offset in `.data` section.

### in case of `modprobe_path`
1. Check the addr of `call_usermodehleper_setup()` from `/proc/kallsyms`. `CONFIG_KALLSYMS` is default to `y`, so you can get it in most cases.
```usemodehelper.txt
/ # cat /proc/kallsyms | grep call_usermodehelper_setup
ffffffff81061cf0 T call_usermodehelper_setup
```

2. Attach GDB and set a break at `call_usemodehelper_setup()`. 
  - This functions's 1st argument is `modprobe_path`.
```call_modprobe.c
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

3. Just invoke `call_modprobe()`. In practice, below command achieves it.
```callit.sh
echo -ne "\xff\xff\xff\xff" > /tmp/nirugiri
chmod +x /tmp/nirugiri
/tmp/nirugiri
```

4. Go to GDB and check the 1st argument. In this case, the target address is `0xffffffff81e33b60`.
```first.sh
No symbol table is loaded.  Use the "file" command.

Breakpoint 1, 0xffffffff81061cf0 in ?? ()
(gdb) p/x $rdi
$1 = 0xffffffff81e33b60
(gdb) x/s $rdi
0xffffffff81e33b60:     "/sbin/modprobe"
```
