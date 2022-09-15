# Useful kernel structures


🚧🚧 Currently, under construct! 🚧🚧

Underscored names are not struct (unnnamed buf or something).

| name | cache size | usage |
|------|------------|-------|
| [`tty_struct`](#tty_struct) | kmalloc-1024 | kbase leak, RIP hijack |
| [`tty_fle_private`](#tty_file_private) | kmalloc-32 | kheap leak |
| [`poll_list, pollfd`](#poll_list) | kmalloc-32 ~ 1024 | kheap leak, arbitrary address free |
| [`user_key_payload`](#user_key_payload) | kmalloc-32 ~ 1024 | arbitrary value write |
| [_`setxattr`](#_setxattr) | kmalloc-32 ~ 1024 | arbitrary value write |
| [`seq_operations`](#seq_operations) | kmalloc-32 | kbase leak, RIP hijack |
| [`subprocess_info`](#subprocess_info) | kmalloc-128 | kbase leak, RIP hijack |

## tty_struct

### abstract

Alloced when `open("/dev/ptmx")` from `kmalloc-1024`, freed when `close("/dev/ptmx")`.

You can leak kbase by reading `tty_struct.ops`, which points to `ptmx_fops`.

You can control RIP by overwriting `tty_struct.ops`.

### note

- `tty_struct.magic` must be `0x5401`, otherwise paranoia check fails.
- `tty_struct.driver` must be valid kptr.
- This is really useful structure for kROP. For detail, reefr to [tty_struct.md](technique/tty_struct.md).

### related challs

- [corjail](https://github.com/smallkirby/pwn-writeups/tree/master/cor2022/corjail) from [CoRCTF2022](https://github.com/Crusaders-of-Rust/corCTF-2022-public-challenge-archive/tree/master/pwn/corjail/task/build)
  - this would be the best example of `tty_struct`
- [klibrary](https://github.com/smallkirby/pwn-writeups/tree/master/3k2021/klibrary/work) from 3kCTF 2021
- [nutty](https://github.com/smallkirby/pwn-writeups/tree/master/union2021/nutty/work) from Union CTF 2021
- [meowmeow](https://github.com/smallkirby/pwn-writeups/tree/master/zer0pts2020/meowmeow) from zer0pts CTF 2020


## tty_file_private

Alloced when `open("/dev/ptmx")` from `kmalloc-32`, freed when `close("/dev/ptmx")`.

You can leak kheap(`kmalloc-1024`) by reading `tty_file_private.tty`, which points to `tty_struct`.

### related challs

- [corjail](https://github.com/smallkirby/pwn-writeups/tree/master/cor2022/corjail) from [CoRCTF2022](https://github.com/Crusaders-of-Rust/corCTF-2022-public-challenge-archive/tree/master/pwn/corjail/task/build)

## poll_list

```c
struct poll_list *         next;                 /*     0     8 */
int                        len;                  /*     8     4 */
struct pollfd              entries[];            /*    12     0 */
```

### abstract

Alloced when `poll()` from `kmalloc-32 ~ 1024`, based on the number of polled fd. Freed when `poll()` finishes, regardless timer expiration or event trigger. `poll_list` contains array of `pollfd` inside it.

`poll_list.next` forms linked list. They are kfreed when `poll()` finishes in order of this linked list. If you can overwrite `next` pointer, you can free arbitrary address, leading to potential UAF.

### related challs

- [corjail](https://github.com/smallkirby/pwn-writeups/tree/master/cor2022/corjail) from [CoRCTF2022](https://github.com/Crusaders-of-Rust/corCTF-2022-public-challenge-archive/tree/master/pwn/corjail/task/build)


## user_key_payload

```c
struct callback_head       rcu;                  /*     0    16 */
short unsigned int         datalen;              /*    16     2 */
char                       data[];               /*    24     0 */
```

### abstract

Alloced when `add_key()` from `kmalloc-32 ~ 1024` based on the size of `data`, freed when `keyctl_revoke(); keyctl_unlink()` by GC. User-given content is embedded in `data`.

### note

- First 16 bytes are not untouched til initialized.
- `user_key_payload.type` is freed by specific GC, so you have to wait for GC to free it after you revoke the key.
- This syscall is prohibited in default docker seccomp filter.

### related challs

- [corjail](https://github.com/smallkirby/pwn-writeups/tree/master/cor2022/corjail) from [CoRCTF2022](https://github.com/Crusaders-of-Rust/corCTF-2022-public-challenge-archive/tree/master/pwn/corjail/task/build)

## _setxattr

### abstraction

Alloced when `setxattr("file", "user.x", data, ...)`, freed right after the call. You can use it to write arbitrary value in almost arbitrary cache.

### note

- The buffer is freed in the same path with allocation, so you can't use it for spray.
- Combining with structs which doesn't initialize itself, you can write arbitrary value in that struct.

### related challs

- [nutty](https://github.com/smallkirby/pwn-writeups/tree/master/union2021/nutty/work) from Union CTF 2021
- [corjail](https://github.com/smallkirby/pwn-writeups/tree/master/cor2022/corjail) from [CoRCTF2022](https://github.com/Crusaders-of-Rust/corCTF-2022-public-challenge-archive/tree/master/pwn/corjail/task/build)
- [Spark](https://github.com/smallkirby/pwn-writeups/tree/master/hitcon2020/spark) from HITCON 2020

## seq_operations

TBD

## subprocess_info

TBD

### related challs

- [SharedHouse](https://github.com/smallkirby/pwn-writeups/tree/master/asis2020quals/shared_house) from ASIS 2020 Quals

## General References

- [Linux useful structures for pwn](https://ptr-yudai.hatenablog.com/entry/2020/03/16/165628#seq_operations) by ptrYudai
