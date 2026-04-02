#include <linux/init.h>   /* needed for macros */
#include <linux/kernel.h> /* needed for debugging */
#include <linux/module.h> /* needed by all modules */

#include <linux/cdev.h>    /* needed for char device driver */
#include <linux/fs.h>      /* needed for device drivers */
#include <linux/uaccess.h> /* needed to copy data to/from user */

#include <linux/device.h> /* needed for sysfs handling */
#include <linux/miscdevice.h>
#include <linux/platform_device.h> /* needed for sysfs handling */

static int val;

ssize_t sysfs_show_val(struct device* dev, struct device_attribute* attr, char* buf) {
    pr_info("sysfs_show_val: val=%d\n", val);
    sprintf(buf, "%d\n", val);
    return strlen(buf);
}

ssize_t sysfs_store_val(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) {
    pr_info("sysfs_store_val: buf=%s\n", buf);
    val = simple_strtol(buf, 0, 10);
    return count;
}

DEVICE_ATTR(val, 0664, sysfs_show_val, sysfs_store_val);

static struct class* sysfs_class;
static struct device* sysfs_device;

static int __init skeleton_init(void) {

    int status = 0;

    sysfs_class = class_create(THIS_MODULE, "my_sysfs_class");
    sysfs_device = device_create(sysfs_class, NULL, 0, NULL, "my_sysfs_device");
    if (status == 0) status = device_create_file(sysfs_device, &dev_attr_val);

    pr_info("Linux module skeleton loaded\n");
    return 0;
}


static void __exit skeleton_exit(void) {

    device_remove_file(sysfs_device, &dev_attr_val);
    device_destroy(sysfs_class, 0);
    class_destroy(sysfs_class);

    pr_info("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR("Fastium <fastium.pro@proton.me>");
MODULE_AUTHOR("Klagarge <remi@heredero.ch>");
MODULE_DESCRIPTION ("Module pilot charachter oriented");
MODULE_LICENSE ("GPL");
