#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/atomic.h>

#define TIMEOUT_S 5

DECLARE_WAIT_QUEUE_HEAD(queue_1);
DECLARE_WAIT_QUEUE_HEAD(queue_2);

static struct task_struct* thread_1;
static struct task_struct* thread_2;
static atomic_t trigger = ATOMIC_INIT(0);

int thread_skeleton_1(void* data) {
    // must wait 5 seconds and start thread 2

    pr_info("Thread 1 started\n");

    while (!kthread_should_stop()) {
        pr_info("Thread 1 wakes up\n");
        ssleep(TIMEOUT_S);

        // Setup trigger for condition of the thread 2
        atomic_set(&trigger, 1);

        // Wake up thread 2
        wake_up_interruptible(&queue_2);

        // Wait until thread 2 has reset the trigger
        wait_event_interruptible(queue_1, atomic_read(&trigger) == 0 || kthread_should_stop());
    }

    return 0;
}

int thread_skeleton_2(void* data) {
    // have to PING when wakes up

    pr_info("Thread 2 started\n");


    while (!kthread_should_stop()) {

        // wait until trigger is set up
        wait_event_interruptible(queue_2, atomic_read(&trigger) == 1 || kthread_should_stop());
        pr_info("Thread 2 wakes up\n");

        // reset trigger
        atomic_set(&trigger, 0);

        // wake up thread 1
        wake_up_interruptible(&queue_1);
    }

    return 0;
}


void sleeping_init(void) {
    pr_info("Initialize kernel thread\n");

    atomic_set(&trigger, 0);

    thread_1 = kthread_run(thread_skeleton_1, NULL, "Thread 1 - sleeping");
    if (IS_ERR(thread_1)) {
        pr_err("Failed to create kernel thread 1\n");
        return;
    }

    thread_2 = kthread_run(thread_skeleton_2, NULL, "Thread 2 - sleeping");
    if (IS_ERR(thread_2)) {
        pr_err("Failed to create kernel thread 1\n");
        return;
    }

    pr_info("Kernel thread sleeping initialized\n");

}

void sleeping_exit(void) {
    pr_info("Exiting kernel sleeping thread\n");
    kthread_stop(thread_1);
    kthread_stop(thread_2);
    pr_info("Kernel thread sleeping exited\n");
}
