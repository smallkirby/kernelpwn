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

## View kernel
I explore kernel on vscode with [ms-vscode.cpptools-extension-pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack) + [Vim](https://marketplace.visualstudio.com/items?itemName=vscodevim.vim) + [CTag](https://marketplace.visualstudio.com/items?itemName=jaydenlin.ctags-support) + [CScope](https://marketplace.visualstudio.com/items?itemName=xulion.codescope) + [C/C++ Intellisence](https://marketplace.visualstudio.com/items?itemName=austin.code-gnu-global).  
For better experience, I recommend to follow below instructions:
- Compile your kernel.
 ```compile.sh
 make menuconfig # or, just make defconfig
 make -j8
 ```
- Generate `compile_commands.json`.
```gen.sh
python3 ./scripts/clang-tools/gen_compile_commands.py
```
- If your directory is for browsing-only purpose, remove compiled objects (I mean you use different dirs/sources for browsing and building kernel). Before removing them, **make sure that you backuped** your generated `compile_commands.json`.
```rm.sh
make clean
```
- Edit VSCode config.
  - Edit config for `C/C++ Extensions`. Config file is `c_cpp_properties.json`. The config file would look like below. Make sure that `/arch/x86/` is your target arch.
    ```c_cpp_properties.json
    {
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/include/**",
                "${workspaceFolder}/arch/x86/**",
                "${default}"
            ],
            "defines": [
                "__GNUC__",
                "__KERNEL__"
            ],
            "compilerPath": "/bin/clang-11",
            "cStandard": "c17",
            "cppStandard": "c++14",
            "intelliSenseMode": "linux-clang-x64",
            "compileCommands": "${workspaceFolder}/compile_commands.cp.json"
        }
    ],
    "version": 4
    }
    ```
  - Edit config for Intellisence if needed. Config file is `compile_commands.json` you generated. Specify the path in `c_cpp_properties.json` as above.
    - In theory if you use `compile_commands.json`, you don't need to specify `includePath`, `defines`, and so on in `c_cpp_properties.json`. If you define them both in `c_cpp_properties.json` and `compile_commands.json`, the latter is used.
- Play [Smash bros](https://www.smashbros.com/), change your theme into gruvbox, which is the best theme in the world, and just wait til everything is indexed.

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

## Debug
now writing...
also refer to [here](../test/)...
