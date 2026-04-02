#include <linux/module.h>  // needed by all modules
#include <linux/init.h>    // needed for macros
#include <linux/kernel.h>  // needed for debugging

#include "ex2.c"


static int __init skeleton_init(void) {
    int ret = 0;

    pr_info("My module loading...\n");
    pr_info("----------------------\n");

    ret = ex2_init();

    pr_info("----------------------\n");
    pr_info("My module is loaded\n");

    return ret;
}

static void __exit skeleton_exit(void) {
    pr_info("My module unloading...\n");
    pr_info("----------------------\n");

    ex2_exit();

    pr_info("----------------------\n");
    pr_info("My module is unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR("Fastium <fastium.pro@proton.me>");
MODULE_AUTHOR("Klagarge <remi@heredero.ch>");
MODULE_DESCRIPTION ("Module pilot charachter oriented");
MODULE_LICENSE ("GPL");
