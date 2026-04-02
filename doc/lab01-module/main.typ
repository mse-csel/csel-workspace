#import "/doc/metadata.typ": *
= Linux Kernel Programming

In this lab, we learn how to develop a tiny kernel module. We initially create a tiny skeleton that just print a message when the module is loaded and unloaded in #ref(<lab01:ex01>, supplement: "Ex").

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



// == Exercises
// #include "ex01.typ"
// #pagebreak()
// #include "ex01.typ"
// #include "ex03.typ"
// #pagebreak()
// #include "ex04.typ"
// #pagebreak()
// #include "ex05.typ"
// #include "ex06.typ"
// #pagebreak()
// #include "ex07.typ"
// #include "ex08.typ"
