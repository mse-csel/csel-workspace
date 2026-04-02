#import "/doc/metadata.typ": *
#let ln(num) = {
  let str_num = if int(num) < 10 { "0" + str(num) } else { str(num) }
  let lbl = label("lab01:ex" + str_num)
  link(lbl)[Ex 1.#num]
}
= Linux Kernel Programming

In this lab, we learn how to develop a tiny kernel module. We initially create a tiny skeleton that just print a message when the module is loaded and unloaded in #ln(1). Then in #ln(2), we see how to use parameters with insmod and with modprobe. To make things easier for us, we’ve added a line to the makefile that copy the module’s configuration file (that contain the parameters for the modules) to the correct directory on the target. The `install` command is used as a combination of `mkdir`, `cp` and `chmod`.
```makefile
install:
	$(MAKE) -C $(KDIR) M=$(PWD) INSTALL_MOD_PATH=$(MODPATH) modules_install
	install -D -m 0644 $(SOURCE).conf $(MODPATH)/etc/modprobe.d/$(SOURCE).conf
```


The exercise 3 ask us what does it mean the 4 values in `/proc/sys/kernel/printk`?
We can show what there is in:
```bash
|> cat /proc/sys/kernel/printk
7	4	1	7
```
The number specified the level of output in a console.

This file specifies the log level for: \
current (7), default (4), minimum (1) and boot-time default (7).

This number matches with this table (#link("https://www.kernel.org/doc/html/latest/core-api/printk-basics.html", [printk documentation])):

#table(
  columns: (2fr, 1fr, 3fr),

  [*Name*], [*String*], [*Alias function*],

  [KERN_EMERG], ["0"], [pr_emerg()],
  [KERN_ALERT], ["1"], [pr_alert()],
  [KERN_CRIT], ["2"], [pr_crit()],
  [KERN_ERR], ["3"], [pr_err()],
  [KERN_WARNING], ["4"], [pr_warning()],
  [KERN_NOTICE], ["5"], [pr_notice()],
  [KERN_INFO], ["6"], [pr_info()],
  [KERN_DEBUG], ["7"], [pr_debug() and pr_devel() if DEBUG is defined],
  [KERN_DEFAULT], [""], [],
  [KERN_CONT], ["c"], [pr_cont()],
)

In #ln(4), we see how to dynamically create elements in the kernel. We use `kcallo` instead of `kzalloc` to allocate all the memory at once and be certain we have the necessary place for all elements of our module. It also a better approach in our opinion to avoid fragmentation.

We spent some time on the #ln(5) to understand that the `request_mem_region` failed because we have an overlap with the EEPROM.

The #ln(6) was a straightforward exercise where we had to develop a module that instantiated a thread.

In the #ln(7) was on concurrency. We had 2 threads with a wait queue. We learn how to suspend a thread, how to wake it up and how to do atomic operation.

In the last exercise of this lab, #ln(8), we see how to manage interruptions and connect them to a gpio.

== Cheat sheet commands
- `modinfo <module.ko>`: display information about a kernel module
- `insmod <module.ko>`: install a kernel module (without checking for dependencies)
- `rmmod <module.ko>`: uninstall a kernel module
- `lsmod`: list the currently loaded kernel modules
- `dmesg`: display the kernel log
- `cat /proc/modules`: display the currently loaded kernel modules with more details
- `modprobe <module>`: install a kernel module and its dependencies
- `modprobe -r <module>`: uninstall a kernel module and its dependencies
- `make`: build the kernel module
- `make install`: install the kernel module in the root filesystem

== Zed
For this lab, we start to work with another code editor than vscode. Not because we don't like Microsoft, ... but mostly for this reason. We use zed with the new devcontainer implementation on this wonderful code editor. To be able to work in nice condition, we add our own `.clangd` build with the help `bear`.

This clangd, allow us to have a perfect autocompletion and a enjoyable code navigation. We can easily jump to the definition of a function and see the documentation of a function.

Thanks to Zed teams for this awesome code editor and \@Fastium for his clangd

== Conclusion
All this lab was done by iteration on the initial skeleton. We develop everything in the #link("https://github.com/Klagarge/MSE-MA-CSEL/tree/main/src/01-skeleton")[src/01-skeleton] folder.

It was a very delightful introduction lab that show us some possibilities when we want to create a kernel module. Everything was new for us, so even it's basics concept, this was a bit challenging to grasp the subject.
