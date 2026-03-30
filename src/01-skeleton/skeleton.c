// skeleton.c
#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/moduleparam.h>	// needed for module parameters

static char* text = "dummy text";
module_param(text, charp, 0664);
static int  number = 1;
module_param(number, int, 0);

static int __init skeleton_init(void)
{
	pr_info ("Linux module skeleton ex02 loaded\n");
	pr_debug ("  text: %s\n  number: %d\n", text, number);
	return 0;
}

static void __exit skeleton_exit(void)
{
	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("Fastium <fastium.pro@proton.me>");
MODULE_AUTHOR ("Klagarge <remi@heredero.ch>");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
