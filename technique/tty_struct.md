# tty_struct

`tty_struct` is a really useful structure for pwn. This document describes what you can by utilizing this structure. Note that this concept is not brand-new and has been used in many CTF challenges and real-world exploit.

## static analysis

First, we dig into the source code of `tty_struct`. Actually, I don't know much about TTY, but knowledge of how the structure is used is enough to pwn. We focus on the initialization and `ioctl` of `/dev/ptmx`.

### initialization and open

`/dev/ptmx` character device is initialized at `/drivers/tty/pty.c`:

```c
static void __init unix98_pty_init(void)
{
  ...
  tty_default_fops(&ptmx_fops);
  ptmx_fops.open = ptmx_open;

  cdev_init(&ptmx_cdev, &ptmx_fops);
  if (cdev_add(&ptmx_cdev, MKDEV(TTYAUX_MAJOR, 2), 1) ||
     register_chrdev_region(MKDEV(TTYAUX_MAJOR, 2), 1, "/dev/ptmx") < 0)
  panic("Couldn't register /dev/ptmx driver");
  device_create(tty_class, NULL, MKDEV(TTYAUX_MAJOR, 2), NULL, "ptmx");
}
```

Corresponding `struct file_operations` shares almost all functions with `tty_fops`, but `.open` member becomes `ptmx_open`.
Therefore, when you `open()` `/dev/ptmx`, `ptmx_open()` is called:

```c
static int ptmx_open(struct inode *inode, struct file *filp)
{
  struct tty_struct *tty;
  int retval;
  ...
  retval = tty_alloc_file(filp);
  ...
  tty = tty_init_dev(ptm_driver, index);
  ...
  tty_add_file(tty, filp);
  ...
}
```

This is where related structures are initialized. First, `tty_alloc_file()` `kmalloc()`s `struct tty_file_private`, and assigns it to `/dev/ptmx`'s `struct file`'s `private_data` member:

```c
int tty_alloc_file(struct file *file)
{
  struct tty_file_private *priv;
  priv = kmalloc(sizeof(*priv), GFP_KERNEL);
  ...
  file->private_data = priv;
  return 0;
}
```

Size of `struct tty_file_private` is 0x20, so `kamalloc-32` is used. Then `tty_init_dev()` is called. We don't follow the detail, but it initializes `struct tty_struct` and returns it. Size of `tty_struct` is 0x2B8, hence it uses `kmalloc-1024` (ofcourse, the size would differ according to build config, but it won't change the size of slab cache). The important members are below:

```c
struct tty_struct {
  int magic; // paranoia check
  struct kref kref; // reference count
  struct device *dev;
  struct tty_driver *driver; // must be valid
  const struct tty_operations *ops; // ptr to vtable
  ...
}
```

After `tty_struct` is allocated by `tty_alloc_file()`, `tty_add_file()` is called toward allocated `tty_struct`:

```c
void tty_add_file(struct tty_struct *tty, struct file *file)
{
  struct tty_file_private *priv = file->private_data;
  priv->tty = tty;
  priv->file = file;
  ...
}
```

It assigns `&tty_struct` to `tty_file_private.tty` member. This is really good, because we can leak the addr of `tty_struct` by reading `tty_file_private`.

### ioctl

When you `ioctl()` for `/dev/ptmx`, `tty_ioctl()` is called. There first, `tty_struct` object is retrieved from `file`

```c
long tty_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  struct tty_struct *tty = file_tty(file);
  if (tty_paranoia_check(tty, file_inode(file), "tty_ioctl"))
    return -EINVAL;
  ...
}

static inline struct tty_struct *file_tty(struct file *file)
{
  return ((struct tty_file_private *)file->private_data)->tty;
}
```

Right after that, paranoia check is executed toward retrieved `tty_struct`. There, magic number in `tty_struct.magic`(`0x5401`) is checked, and returns immediately if the magic number is detected to be corrupted.
After the check, execution continues depending on `cmd` of `ioctl`. If there are no much for specified `cmd`, it reaches below path:

```c
switch (cmd) {
...
default:
  if (tty->ops->ioctl) {
    retval = tty->ops->ioctl(tty, cmd, arg);
    if (retval != -ENOIOCTLCMD)
    return retval;
  }
}
```

It calls `tty->ops->ioctl` if not NULL. This is where we can hijack the control flow.

## What you can do

The main principle you can achieve by utilizing `tty_struct` is kheap leak and control flow hijacking.

### kheap leak

Suppose you have below conditions:

- You have UAF in `kmalloc-32`
- You have UAF in `kmalloc-1024`
- You have write primitive in `kmalloc-1024` (full write primitive, or partial write primitive)

If you have UAF in `kmalloc-32`, **you can leak kheap addr of `kmalloc-32` via `tty_file_private`**. This is because `tty_file_private` has addr of `tty_struct` inside it, which is allocated in `kmalloc-1024`.

If you have UAF in `kmalloc-1024`, **you can leak kbase via `tty_struct.ops`**. This is because `tty_struct.ops` have addr of `ptmx_fops`.

### RIP hijacking by kROP on `tty_struct` itself

This is the main part of this description, and where you can understand how this structure is precious for exploitation.

Suppose you have sequential write primitive on `tty_struct` for at least `0x200` bytes. Basically, you can get RIP by overwriting `tty_struct.ops`. There, you have to be carefull that you don't corrupt `tty_struct.magic`, otherwise `tty_ioctl()` would return immediately without `jmp`-ing to `tty_struct.ops`.
By the way, `jmp`-ing to arbitrary addr is not enough to control RIP in many cases. This is due to `KPTI` mechanism. You cannot execute your own program in userland if `KPTI` is enabled. You may want to do stack pivot. This structure is suitable to do stack pivot.

What you have to do is to prepare ROP chains in heap, then execute `mov rsp, <new stack where ROP is built>`. But the useful gadget which does `mov rsp, <arbitrary value>` would not exist in kernel image. So **we need to ROP** before actual ROP...! And you can achieve this *pre-ROP* only within `tty_struct`!

Below, we suppose we have sequential write primitive at the top of `tty_struct` for `0x200` bytes. First, you have to write `0x5401` QWORD to bypass paranoia check in `tty_ioctl()`.
Then, you write an addr of *fake vtable* at `ops`, which contains addr you want to execute. You can write this vtable in `tty_struct` itself, such as at `+0x50`. `tty_struct` is, as far as I know, *strong* structure, which does not invoke kernel oops even if we corrupt some data in it. So you can write arbitrary data in `tty_struct` without any problem. The only exception is `magic` must be `0x5401`, and **`driver`** must be valid heap addr. But you can bypass both of them (explained below for the latter).

`tty_ioctl` would `jmp` to the gadget addr you wrote in *fake vtable* at `+0x50`.  The best gadget would be `leave` in this case. This is because ***`RBP` points to `tty_struct` itself right after this jmp***. So `leave` would do `mov rsp, rbp` and `pop rbp`, which make `RSP` point to the top of `tty_struct` itself, and make `RBP` become `magic|kref`. After `leave`, `dev` becomes return addr of this gadget because `RSP` is now pointing to `tty_struct + 0x08`. So you can write addr of `pop rsp` gadget as `dev`. This way, `return` would returns to `pop rsp` and pops the value of `driver` as `RSP`. You can write `tty_struct + 0x170` as `driver` and write actuall ROP chain at `tty_struct + 0x170`. Remember that `driver` must be valid heap addr to work without kernel oops, but you can bypass this limitation by double-phased ROP boot!!

```txt
struct tty_struct {
  +000: magic, kref : 0x5401
  +008: dev;        : `pop rsp` gadget (return addr of `leave`)
  +010: driver;     : &tty_struct + 0x170 (`pop rsp`ed) (MUST be valid kheap addr)
  +018: ops;        : &tty_struct + 0x50
  +020: index;
  ...
  +050:             : fake vtable (size=0x120) (points to `leave` gadget)
  ...
  +170:             : actual ROP chain
  ...
}
```

## Related CTF challs

- [corjail](https://github.com/smallkirby/pwn-writeups/tree/master/cor2022/corjail) from [CoRCTF2022](https://github.com/Crusaders-of-Rust/corCTF-2022-public-challenge-archive/tree/master/pwn/corjail/task/build)
  - this would be the best example of `tty_struct`
- [klibrary](https://github.com/smallkirby/pwn-writeups/tree/master/3k2021/klibrary/work) from 3kCTF 2021
- [nutty](https://github.com/smallkirby/pwn-writeups/tree/master/union2021/nutty/work) from Union CTF 2021
- [meowmeow](https://github.com/smallkirby/pwn-writeups/tree/master/zer0pts2020/meowmeow) from zer0pts CTF 2020
