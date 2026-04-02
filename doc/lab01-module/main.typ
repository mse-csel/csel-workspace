#import "/doc/metadata.typ": *
#let ln(num) = {
  let str_num = if int(num) < 10 { "0" + str(num) } else { str(num) }
  let lbl = label("lab01:ex" + str_num)
  link(lbl)[Ex 1.#num]
}
= Linux Kernel Programming

In this lab, we learn how to develop a tiny kernel module. We initially create a tiny skeleton that just print a message when the module is loaded and unloaded in #ln(1). Then in #ln(2), we see how to use parameters with insmod and with modprobe. To make things easier for us, we’ve added a line to the makefile that copy the module’s configuration file (that contain the parameters for the modules) to the correct directory on the target. The `install` command is used as combination of `mkdir`, `cp` and `chmod`.
```makefile
install:
	$(MAKE) -C $(KDIR) M=$(PWD) INSTALL_MOD_PATH=$(MODPATH) modules_install
	install -D -m 0644 $(SOURCE).conf $(MODPATH)/etc/modprobe.d/$(SOURCE).conf
```


The exercice 3 ask us what does it mean the 4 values in ```/proc/sys/kernel/printk``` ?
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

In #ln(4), we see how to dynamically create elements in the kernel.

== Cheatsheet commands
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
