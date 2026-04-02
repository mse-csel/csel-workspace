#import "/doc/metadata.typ": *

=== Generate kernel module out of tree <lab01:ex01>

#colorbox(title: "Exercise", color: hei-blue)[
  Create the skeleton of a kernel module and generate it outside the kernel sources using a Makefile. The module should display a message when it is registered and when it is uninstalled.
]
//--------------

We already have a skeleton in `src/02-modules/exercice01` (now `solutions/02_modules/exercice01` that we move to `src/01-skeleton`). We see on the Makefile that the module is generated outside the kernel sources with the `KDIR` variable imported from `src/kernel_settings`. This variable point to the kernel sources.
The Makefile also use the `PWD` variable to the current directory.
The `make` command will use these variables to generate the module in the current directory.

```makefile
$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(CPU) CROSS_COMPILE=$(TOOLS) modules
```

//--------------
#colorbox(title: "Exercise", color: hei-blue)[
  Test on the host machine the command modinfo1 on your module skeleton and compare the information returned with that of the source code.
]

```bash
|> modinfo mymodule.ko
filename:       /workspace/src/01-skeleton/mymodule.ko
license:        GPL
description:    Module skeleton
author:         Klagarge <remi@heredero.ch>
author:         Fastium <fastium.pro@proton.me>
depends:
name:           mymodule
vermagic:       5.15.148 SMP preempt mod_unload aarch64
parm:           text:charp
parm:           elements:int
```
//--------------
#colorbox(title: "Exercise", color: hei-blue)[
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
#colorbox(title: "Exercise", color: hei-blue)[
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
The `/proc/modules` file give us more details about the state of the module. We see it is now live (charged in memory and running)

//--------------
#colorbox(title: "Exercise", color: hei-blue)[
  Uninstall the module (rmmod).
]

```bash
|> rmmod mymodule.ko
[ 2989.535793] Linux module skeleton unloaded
```

//--------------
#colorbox(title: "Exercise", color: hei-blue)[
  Adapt the Makefile of the module to allow the installation of the module with other kernel modules allowing the use of the modprobe command. The module should be installed in the root filesystem used in cifs by the target.
]

```bash
# On host:
|> make install

# On target:
|> modprobe mymodule
[ 3359.811183] Linux module 01 skeleton loaded
```
