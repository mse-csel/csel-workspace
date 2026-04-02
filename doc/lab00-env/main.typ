= Embedded Linux Environment

In this laboratory, we see how to setup our environnement and how to have several way to boot. That include a `boot.cifs` that allow us to load the rootfs from samba to easily share the rootfs between the host and the target. And also a `boot.tftp` that allow us to load the kernel by tftp, which is really usefull when we want to modify the kernel and test it without having to reflash the whole system.

We also see how to debug our system with a remote debugger. That allow us to use debug in our code editor (vscode) a programm that run on the target.

#figure(
  image("./dev-environment.png"),
  caption: "Development environment schema"
) <fig:dev-env>


== Questions
=== How to generate U-Boot?
We use buildroot, a tool to build embedded Linux.
It can generate the U-Boot bootloader.
With `make menuconfig`, we can select the U-Boot package.
U-boot can be configured with `make uboot-menuconfig`

And finally, when we have configured everything, we can build the whole system with `make` command.
Or only uboot with `make uboot` command.

=== How to add and build a additional package in Buildroot?
In buildroot, with `make menuconfig`, we can select the package we want in `Target packages` section. We can specifically build it with `make <package-name>` command. Otherwise, it will be built with the whole system when we run `make` command.

=== How to modify the Linux kernel configuration?
Like all package, with `make <package-name>-menuconfig` command. So, for Linux kernel:
```bash
|> make linux-menuconfig
```

=== How to generate a custom rootfs?
First of all, select the type of filesystem you want to generate in `Filesystem images` section of `make menuconfig`. We can use an overlay to customise our rootfs.
The overlay is a directory (in the board folder) with the same structure as rootfs and it will merge with the generated rootfs. So, we can add files and directories in the overlay and they will be added to the final rootfs.


=== How to use the eMMC card instead of the SD card?
We need to change the boot script `(boot*.cmd)` to load from eMMC by changing the `fatload` command with the correct number. Probably 1 instead of 0.
```
fatload mmc 1 $kernel_addr_r Image
```


=== In cours support, we find several configurations of the development environment. What would be the optimal configuration for developing only user-space applications?
If we develop only user space program, we don't need to load kernel by tftp. But it's really usefull to have rootfs load by samba. So the best approach is to use the `boot.cifs`.
