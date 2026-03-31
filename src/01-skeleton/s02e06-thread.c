#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging


#include <linux/kthread.h>
#include <linux/delay.h>

static struct task_struct* sample_thread;

#define DELAY_S 5

int thread_skeletonThread (void* data) {

    pr_info("Thread started\n");

    while (!kthread_should_stop()) {
        pr_info("PING!\n");
        ssleep(DELAY_S);
    }

    return 0;
}

void thread_init(void) {
    pr_info("Initialize kernel thread\n");

    sample_thread = kthread_run(thread_skeletonThread, NULL, "The Machine that goes");
    if (IS_ERR(sample_thread)) {
        pr_err("Failed to create kernel thread\n");
        return;
    }

    pr_info("Kernel thread initialized\n");

}

void thread_exit(void) {
    pr_info("Exiting kernel thread\n");
    kthread_stop(sample_thread);
    pr_info("Kernel thread exited\n");
}
