// skeleton.c
#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include "s02e02-parameters.c"
#include "s02e04-dynamic_allocation.c"
#include "s02e05-io_memory_mapped.c"
#include "s02e06-thread.c"


static int __init skeleton_init(void) {
    pr_info("Linux module skeleton ex05 loading...\n");
	pr_info("--------------------\n");

    // Lab02 - Exercise 2: Parameters
    parameters_print();

    pr_info("--------------------\n");

    // Lab02 - Exercise 4: Dynamic memory allocation and linked list
    dynAlloc_init();

    pr_info("--------------------\n");

    // Lab02 - Exercise 5: Memory-mapped I/O
    ioMemoryMapped_init();

    pr_info("--------------------\n");

    // Lab02 - Exercise 6: Kernel thread
    thread_init();

    pr_info("--------------------\n");

    pr_info("Linux module skeleton loaded\n");
	return 0;
}

static void __exit skeleton_exit(void) {

    
    // Lab02 - Exercise 4: Dynamic memory allocation and linked list
    dynAlloc_exit();

    pr_info("--------------------\n");

    // Lab02 - Exercise 5: Memory-mapped I/O
    ioMemoryMapped_exit();

    pr_info("--------------------\n");

    // Lab02 - Exercise 6: Kernel thread
    thread_exit();

    pr_info("--------------------\n");

    pr_info ("Linux module skeleton unloaded\n");
}

module_init(skeleton_init);
module_exit(skeleton_exit);

MODULE_AUTHOR("Fastium <fastium.pro@proton.me>");
MODULE_AUTHOR("Klagarge <remi@heredero.ch>");
MODULE_DESCRIPTION("Module skeleton");
MODULE_LICENSE("GPL");
