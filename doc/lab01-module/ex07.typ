#import "/doc/metadata.typ": *

=== Sleeping  <lab01:ex07>
#colorbox(title: "Exercise", color: hei-blue)[
  Develop a module which instanciate 2 threads in the kernel. The first one will wait a wake up notification from the second thread and will sleep. The second will send the notification every 5 seconds. Then it will sleep. We will use the waitqueue for the sleeping function. To allow debugging, each thread will send a message when it wakes up.
]

This exercice make 2 threads in concurrency with wait queue. Here the queue ware declare
statically with the macro `DECLARE_WAIT_QUEUE_HEAD`.  Then for this exercice we use an atomic
trigger with 2 queues. It important that the trigger is atomic or protected by mutex because
there is concurrency. The wait queues are used to wait until the trigger has changed to keep
synchronization between the threads.

It is very important to add `kthread_should_stop()` as a condition to wake up queue, because if there is
a problem during the implementation, we cannot kill the code.
