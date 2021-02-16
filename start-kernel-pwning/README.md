# Start Kernel Pwning
## About
This dir contains how to start kernel-pwn mainly for beginners **including me**.

## Fetch kernel src
We can refer to Linux kernel source code on the Internet such as on [bootlin](https://elixir.bootlin.com/linux/latest/source/kernel).
However, having them locally is not bad for quick and detailed reference.
- Clone [repo](https://github.com/torvalds/linux).
- checkout on any tag or commit as you like.

## Build kernel
You can build kernel from source code cloned above, or you can use buildroot for convenience.
- [Download buildroot](https://buildroot.org/download.html)
- untar it and make config
  - use default config for x64:
    - ```config_x64.sh
      make list-defconfigs
      make qemu_x86_64_defconfig
      ```
  - custom config by yourself:
    - ```config-custom.sh
      make allnoconfig
      make menuconfig
      ```
    - Example of config is [here](../config/.config).
- make config for kenerl
  - You can choose any version of kernel to build.
    - don't forget to choose below settings
      - ```make-menuconfig.sh
        Toolchain -> Kernel Headers -> Same as kernel being built
        Kernel -> Kernel Version -> <As you want>
        Kernel -> Kernel configuration -> Use the architecture default configuration
  - type `make linux-menuconfig` and custom as you like.
  - Example of config is [here](../config/.config.linux)
- finally, build the kernel.
  - `make -j<your max logical core number>`
  - the outputs are placed on `output/images/`.


## run kernel
Sample QEMU run script is [here](../test/).
```qemu.sh
qemu-system-x86_64 \
  -kernel ./bzImage \
  -initrd ./rootfs.cpio \
  -nographic \
  -monitor none \
  -cpu qemu64 \
  -append "console=ttyS0 nokaslr panic=1" \
  -no-reboot \
  -s \
  -m 256M
```

## Under Construction
now writing...
also refer to [here](../test/)...