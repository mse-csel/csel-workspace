// #import "@preview/hei-synd-report:0.1.1": *
#import "@preview/hei-synd-thesis:0.3.1": *
#import "/doc/metadata.typ": *
#import "/doc/resources/glossary.typ": *
#show:make-glossary
#register-glossary(entry-list)

#import "@preview/fractusist:0.1.1":*

#import "@preview/grape-suite:3.1.0": exercise
#import exercise: task, subtask

//-------------------------------------
// Template config
//

#{
  doc.title = "Environment & Kernel Programming"
  doc.subtitle = [
    Lab 01: Embedded Linux Environment \
    & \
    Lab 02: Linux Kernel Programming
  ]
  doc.logos.tp_main = dragon-curve(
    10,
    step-size: 10,
    stroke-style: stroke(
      paint: gradient.radial(..color.map.rocket),
      thickness: 3pt, join: "round"),
    height: 10cm,
  )
  doc.version = "v0.1.0"
}

#show: report.with(
  option: option,
  doc: doc,
  date: date,
  tableof: tableof,
)
#v(5em)
#infobox()[
  The repository for this labs can be found at the following address:

  #align(center)[https://github.com/Klagarge/MSE-MA-CSEL]
]
#pagebreak()

//-------------------------------------
// Content
//

= Embedded Linux Environment

In this laboratory, we see how to setup our environnement and how to have several way to boot. That include a `boot.cifs` that allow us to load the rootfs from samba to easily share the rootfs between the host and the target. And also a `boot.tftp` that allow us to load the kernel by tftp, which is really usefull when we want to modify the kernel and test it without having to reflash the whole system.

We also see how to debug our system with a remote debugger. That allow us to use debug in our code editor (vscode) a programm that run on the target.

// PLACEHOLEDER SCHEMA


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
The overlay is a directory (inin the board folder) with the same structure as rootfs and it will merge with the generated rootfs. So, we can add files and directories in the overlay and they will be added to the final rootfs.


=== How to use the eMMC card instead of the SD card?
We need to change the boot script `(boot*.cmd)` to load from eMMC by changing the `fatload` command with the correct number. Probably 1 instead of 0.
```
fatload mmc 1 $kernel_addr_r Image
```


=== In cours support, we find several configurations of the development environment. What would be the optimal configuration for developing only user-space applications?
If we develop only user space program, we don't need to load kernel by tftp. But it's really usefull to have rootfs load by samba. So the best approach is to use the `boot.cifs`.



//--------------------------------------
#pagebreak()
= Linux Kernel Programming

== Exercises

//-------------------
// Exercise 1: Generate kernel module out of tree
//-------------------
#task(
  [Generate kernel module out of tree],
  [],
)

//--------------
#subtask[
  Create the skeleton of a kernel module and generate it outside the kernel sources using a Makefile. The module should display a message when it is registered and when it is uninstalled.
]

We already have a skeleton in `src/02-modules/exercice01`. We see on the Makefile that the module is generated outside the kernel sources with the `KDIR` variable imported from `src/kernel_settings`. This variable point to the kernel sources.
The Makefile also use the `PWD` variable to the current directory.
The `make` command will use these variables to generate the module in the current directory.

```makefile
$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(CPU) CROSS_COMPILE=$(TOOLS) modules
```

//--------------
#subtask[
  Test on the host machine the command modinfo1 on your module skeleton and compare the information returned with that of the source code.
]

```bash
|> modinfo mymodule.ko
filename:       /workspace/src/02_modules/exercice01/mymodule.ko
license:        GPL
description:    Module skeleton
author:         Daniel Gachet <daniel.gachet@hefr.ch>
depends:
name:           mymodule
vermagic:       5.15.148 SMP preempt mod_unload aarch64
parm:           text:charp
parm:           elements:int
```
//--------------
#subtask[
  Install the module (insmod) and check the kernel log (dmesg)
]

```bash
|> insmod mymodule.ko
[ 1727.896902] mymodule: loading out-of-tree module taints kernel.
[ 1727.903442] Linux module 01 skeleton loaded
```
We can see the module is indead out-of-tree and correctly loaded.
```bash
|> dmesg | tail -5
[ 1381.694764] CIFS: Attempting to mount \\192.168.53.4\workspace
[ 1727.896902] mymodule: loading out-of-tree module taints kernel.
[ 1727.903442] Linux module 01 skeleton loaded
[ 1727.907659]   text: dummy text
[ 1727.907659]   elements: 1
```

//--------------
#subtask[
  Compare the results obtained by the lsmod command with those obtained with the cat /proc/modules command
]

```bash
|> lsmod
Module                  Size  Used by    Tainted: G
mymodule               16384  0
···

|> cat /proc/modules
mymodule 16384 0 - Live 0xffff8000011bf000 (O)
···
```

//--------------
#subtask[
  Uninstall the module (rmmod).
]

```bash
|> rmmod mymodule.ko
[ 2989.535793] Linux module skeleton unloaded
```

//--------------
#subtask[
  Adapt the Makefile of the module to allow the installation of the module with other kernel modules allowing the use of the modprobe command. The module should be installed in the root filesystem used in cifs by the target.
]

```bash
# On host:
|> make install

# On target:
|> modprobe mymodule
[ 3359.811183] Linux module 01 skeleton loaded
```

//-------------------
// Exercise 2: Adapt the kernel module to receive parameters
//-------------------
#task(
  [Adapt the kernel module to receive parameters],
  [
    Adapt the kernel module of the previous exercise to receive two or three parameters of your choice. These parameters will be displayed in the console when the module is loaded.
  ],
)

//-------------------
// Exercise 3: What does it mean the 4 values in ```/proc/sys/kernel/printk``` ?
//-------------------
#task(
  [What does it mean the 4 values in ```/proc/sys/kernel/printk``` ?],
  [
    ```bash
    |> cat /proc/sys/kernel/printk
    7	4	1	7
    ```
    The number specified the level of output in a console.

    This file specifies the log level for:
    - current: 7
    - default: 4
    - minimum: 1
    - boot-time-default: 7

    This number matches with this table (#link("https://www.kernel.org/doc/html/latest/core-api/printk-basics.html", [printk documentation])):
    #image("resources/img/printk-log-levels.png")
  ]
)


//-------------------------------------
// Glossary
//
#heading(numbering:none, outlined: false)[] <sec:end>
#make_glossary(gloss:gloss, title:i18n("gloss-title"))
