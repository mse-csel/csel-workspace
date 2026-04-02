#import "/doc/metadata.typ": *
#let ln(num) = {
  // let str_num = if int(num) < 10 { "0" + str(num) } else { str(num) }
  // let lbl = label("lab02:ex" + str_num)
  // link(lbl)[Ex 2.#num]
  [Ex 2.#num]
}

= Linux Kernel Programming

In the First exercice, we learn how to access a register trought the `/dev/mem` interface. The purpose was to read the chip ID but we learn how to access in a specific region of the memory. How pages work and how to map them in the user space.

For exercice 2, we see how to create a character device driver. We learn how to create a device file, how to write a read and write functions and how to test it with `echo` and `cat`. Our module have a `MAJOR` dynamically allocated (but should be 511 with default nanopi installation) and only one minor. To verify the major number, we can use `cat /proc/devices` and look for our module name. To test the module, we need to create a character device file with the right major and minor number. 
```bash
mknod /dev/test-device c 511 0 # Create character device 
echo "lalalalalaalalalalallala" > /dev/test-device # Write to the device
cat /dev/test-device # Read from the device
```
Quite easy to extend to exercice 3 by adding the parameters as we did in the previous lab. This parameters define the number of minor available. 

Exercice 4 is the continuity, we had to create a tiny app that basically do the `echo` and `cat` for us. We can use the `open`, `write`, `read` and `close` system calls to interact with our device file. We still need to create the device file: 
```bash
mknod /dev/toto0 c 511 0
```



== Adaptation for Zed environment
For this lab we have to work with application and not with module. We have the same problem with clang for the LSP with Zed. To solve it, we include the linux header files and specify the path of sysroot. Like this, clang have all the dependencies that we need. And tadam, the wonderful envionment we had on previous lab is back!

== Conclusion
All the content of this is on #link("https://github.com/Klagarge/MSE-MA-CSEL/tree/main/src/02-driver")[src/02-driver]. It was pleasent to initially see how to manage a character device manually and step by step see how to do it with an easier methods. I personnaly like to start from the bottom. 