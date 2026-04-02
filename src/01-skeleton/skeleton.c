// skeleton.c
#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include "linux/printk.h"
#include "kernel-module/s02e02-parameters.c"
// #define PARAMETERS

#include "kernel-module/s02e04-dynamic_allocation.c"
// #define DYNAMIC_ALLOCATION

#include "kernel-module/s02e05-io_memory_mapped.c"
// #define IO_MEMORY_MAPPED

#include "kernel-module/s02e06-thread.c"
// #define THREAD

#include "kernel-module/s02e07-sleeping.c"
// #define SLEEPING

#include "kernel-module/s02e08-interrupt.c"
#define INTERRUPT


static int __init skeleton_init(void) {
    pr_info("Linux module skeleton ex05 loading...\n");

    // Lab02 - Exercise 2: Parameters
    #ifdef PARAMETERS
	pr_info("--------------------\n");
    parameters_print();
    #endif

    // Lab02 - Exercise 4: Dynamic memory allocation and linked list
    #ifdef DYNAMIC_ALLOCATION
    pr_info("--------------------\n");
    Alloc_init();
    #endif

    // Lab02 - Exercise 5: Memory-mapped I/O
    #ifdef IO_MEMORY_MAPPED
    pr_info("--------------------\n");
    ioMemoryMapped_init();
    #endif

    // Lab02 - Exercise 6: Kernel thread
    #ifdef THREAD
    pr_info("--------------------\n");
    thread_init();
    #endif

    // Lab02 - Exercise 7: Sleeping
    #ifdef SLEEPING
    pr_info("--------------------\n");
    sleeping_init();
    #endif

    // Lab02 - Exercise 8: Interrupt
    #ifdef INTERRUPT
    pr_info("--------------------\n");
    interrupt_init();
    #endif

    pr_info("--------------------\n");
    pr_info("Linux module skeleton loaded\n");
	return 0;
}

static void __exit skeleton_exit(void) {

    pr_info("Linux module skeleton unloading...\n");

    // Lab02 - Exercise 4: Dynamic memory allocation and linked list
    #ifdef DYNAMIC_ALLOCATION
    pr_info("--------------------\n");
    dynAlloc_exit();
    #endif

    // Lab02 - Exercise 5: Memory-mapped I/O
    #ifdef IO_MEMORY_MAPPED
    pr_info("--------------------\n");
    ioMemoryMapped_exit();
    #endif

    // Lab02 - Exercise 6: Kernel thread
    #ifdef THREAD
    pr_info("--------------------\n");
    thread_exit();
    #endif

    // Lab02 - Exercise 7: Sleeping
    #ifdef SLEEPING
    pr_info("--------------------\n");
    sleeping_exit();
    #endif

    // Lab02 - Exercise 8: Interrupt
    #ifdef INTERRUPT
    pr_info("--------------------\n");
    interrupt_exit();
    #endif

    pr_info("--------------------\n");
    pr_info ("Linux module skeleton unloaded\n");
}

module_init(skeleton_init);
module_exit(skeleton_exit);

MODULE_AUTHOR("Fastium <fastium.pro@proton.me>");
MODULE_AUTHOR("Klagarge <remi@heredero.ch>");
MODULE_DESCRIPTION("Module skeleton");
MODULE_LICENSE("GPL");
