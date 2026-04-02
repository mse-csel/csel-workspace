#import "/doc/metadata.typ": *

=== Kernel thread  <lab01:ex06>
#colorbox(title: "Exercise", color: hei-blue)[
  Develop a module which allows to instanciate a thread in the kernel. This thread will display a message every 5 seconds. Use the function ```ssleep(5)``` to sleep the thread from ``` linux/delay.h```.
]

Easy exercice, a thread in the kernel is a `struct task_struct*` that can be created with `kthread_run`
