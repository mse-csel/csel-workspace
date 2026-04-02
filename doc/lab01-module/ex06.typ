#import "/doc/metadata.typ": *

#task(
  [
    Kernel thread
  ],
  [
    Develop a module which allows to instanciate a thread in the kernel. This thread will display a message every 5 seconds. Use the function ```ssleep(5)``` to sleep the thread from ``` linux/delay.h```.

  ]
)

Easy exercice, a thread in the kernet is a `struct task_struct*` that can be created with `kthread_run`
