#import "/doc/metadata.typ": *

=== Adapt the kernel module to receive parameters <lab01:ex02>
#colorbox(title: "Exercise", color: hei-blue)[
  Adapt the kernel module of the previous exercise to receive two or three parameters of your choice. These parameters will be displayed in the console when the module is loaded.
]

```bash
|> modprobe mymodule
[ 3583.616662] Linux module skeleton ex02 loaded
|> dmesg | tail -5
[ 3559.279143]   number: 1
[ 3581.198562] Linux module skeleton unloaded
[ 3583.616662] Linux module skeleton ex03 loaded
[ 3583.621085]   text: The answer to the Ultimate Question of Life, The Universe, and Everything
[ 3583.621085]   number: 42
|> modprobe -r mymodule
[ 3588.404778] Linux module skeleton unloaded
```
