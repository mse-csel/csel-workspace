/* skeleton.c */
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
static struct kobject *stats_kobj;
static int read_count = 0;
static int write_count = 0;

static ssize_t show_read_count(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", read_count);
}
static struct kobj_attribute obj_attr_read_count = __ATTR(read_count, 0444, show_read_count, NULL);

static ssize_t show_write_count(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", write_count);
}
static struct kobj_attribute obj_attr_write_count = __ATTR(write_count, 0444, show_write_count, NULL);

ssize_t val_show(struct device* dev,
                       struct device_attribute* attr,
                       char* buf)
{
    sprintf(buf, "%d\n", val);
    read_count++;
    return strlen(buf);
}

ssize_t val_store(struct device* dev,
                        struct device_attribute* attr,
                        const char* buf,
                        size_t count)
{
    val = simple_strtol(buf, 0, 10);
    write_count++;
    return count;
}
DEVICE_ATTR_RW(val);

static struct class* sysfs_class;
static struct device* sysfs_device;

static int __init skeleton_init(void)
{
    int status = 0;

    sysfs_class = class_create(THIS_MODULE, "my_sysfs_class");
    sysfs_device = device_create(sysfs_class, NULL, 0, NULL, "my_sysfs_device");
    if (status == 0) status = device_create_file(sysfs_device, &dev_attr_val);

    // Create a subdirectory under the device
    stats_kobj = kobject_create_and_add("stats", &sysfs_device->kobj);
    if (!stats_kobj) {
        pr_err("Failed to create stats directory\n");
        return -ENOMEM;
    }

    // Add attributes to the stats subdirectory
    status = sysfs_create_file(stats_kobj, &obj_attr_read_count.attr);
    if (status) {
        pr_err("Failed to create read_count attribute\n");
        kobject_put(stats_kobj);
        return status;
    }

    status = sysfs_create_file(stats_kobj, &obj_attr_write_count.attr);
    if (status) {
        pr_err("Failed to create write_count attribute\n");
        kobject_put(stats_kobj);
        return status;
    }

    pr_info("Linux module skeleton loaded\n");
    return 0;
}

static void __exit skeleton_exit(void)
{
    // Remove stats subdirectory and its attributes
    if (stats_kobj) {
        sysfs_remove_file(stats_kobj, &obj_attr_read_count.attr);
        sysfs_remove_file(stats_kobj, &obj_attr_write_count.attr);
        kobject_put(stats_kobj);
    }
    device_remove_file(sysfs_device, &dev_attr_val);
    device_destroy(sysfs_class, 0);
    class_destroy(sysfs_class);

    pr_info("Linux module skeleton unloaded\n");
}

module_init(skeleton_init);
module_exit(skeleton_exit);

MODULE_AUTHOR("Bastien Veuthey <bastien.veuthey@hefr.ch>");
MODULE_DESCRIPTION("Module skeleton stats");
MODULE_LICENSE("GPL");
