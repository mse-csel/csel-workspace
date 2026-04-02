#include <linux/module.h>  // needed by all modules
#include <linux/moduleparam.h>
#include <linux/init.h>    // needed for macros
#include <linux/kernel.h>  // needed for debugging
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/minmax.h>
#include <stddef.h>
#include <linux/uaccess.h>
#include <linux/slab.h> // dynamic memory allocation


// linux theory: https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html


#define MY_MAJOR       42
#define MY_MAX_MINORS  5

#define BUFFER_SIZE 300

// setup as argument the number of buffer available in the device
static int instances = 3;
module_param(instances, int, 0);

struct my_device_data {
    dev_t dev_t;
    struct cdev cdev;
    /* my data starts here */
    char** buffers;
};

struct my_device_data devs;

// inode: https://www.kernel.org/doc/html/latest/filesystems/ext4/inodes.html
// file: https://docs.kernel.org/filesystems/api-summary.html#c.file

int skeleton_open(struct inode* i, struct file* f) {

    pr_info("Open file \n major:%d\n minor:%d\n",
            imajor(i),
            iminor(i));

    if (iminor(i) >= instances) {
        return -EFAULT;
    }

    if ((f->f_mode & (FMODE_READ | FMODE_WRITE)) != 0) {
        pr_info("skeleton : opened for reading & writing...\n");
    } else if ((f->f_mode & FMODE_READ) != 0) {
        pr_info("skeleton : opened for reading...\n");
    } else if ((f->f_mode & FMODE_WRITE) != 0) {
        pr_info("skeleton : opened for writing...\n");
    }

    // Get the stuct of my data
    struct my_device_data *my_data = container_of(i->i_cdev, struct my_device_data, cdev);

    // point private data on driver data, here this is a char buffer
    // point on the right minor buffer
    f->private_data = my_data->buffers[iminor(i)];

    return 0;
}

int skeleton_release(struct inode* i, struct file* f) {

    pr_info("Release file\n");

    return 0;
}

ssize_t skeleton_read(struct file* f, char* __user buf, size_t count, loff_t* off) {

    pr_info("Read file\n");

    ssize_t len = min(BUFFER_SIZE - ((size_t)*off), count);

    if (len <= 0) {
        pr_info("Cannot read data for length: %ld\n", len);
        return 0;
    }


    /* read data from my_data->buffer to user buffer */
    if (copy_to_user(buf, f->private_data + *off, len)) {
        pr_info("Failed to copy to user space buffer\n");
        return -EFAULT;
    }

    *off += len;
    return len;

}

ssize_t skeleton_write(struct file* f, const char* __user buf, size_t count, loff_t* off) {

    pr_info("Write file\n");

    ssize_t len = min(BUFFER_SIZE - ((size_t)*off), count);

    if (len <= 0) {
        pr_info("Cannot write data for length: %ld\n", len);
        return 0;
    }


    /* read data from user buffer to my_data->buffer */
    if (copy_from_user(f->private_data + *off, buf, len)) {
        pr_info("Failed to copy from user space buffer\n");
        return -EFAULT;
    }


    *off += len;
    return len;
}


static struct file_operations skeleton_fops = {
    .owner   = THIS_MODULE,
    .open    = skeleton_open,
    .read    = skeleton_read,
    .write   = skeleton_write,
    .release = skeleton_release,
};

static int __init skeleton_init(void) {
    int ret = 0;

    pr_info("My module loading...\n");
    pr_info("----------------------\n");

    pr_info("Load exercice 2\n");

    // ret = register_chrdev_region(MKDEV(MY_MAJOR, 0), instances, "My module"); // register statically

    ret = alloc_chrdev_region(&devs.dev_t, 0, instances, "mymodule"); //allocate major and minor

    if (ret != 0) {
        /* report error */
        pr_info("Module registration error: %d\n", ret);
        return ret;
    }

    /* initialize devs fields */
    cdev_init(&devs.cdev, &skeleton_fops); // initialize device with files operations
    ret = cdev_add(&devs.cdev, devs.dev_t, instances); // notify kernel

    if (ret != 0) {
        /* report error */
        pr_info("cdev add error: %d\n", ret);
        return ret;
    }

    // allocate the array of buffer
    int i;
    devs.buffers = kzalloc(sizeof(char*) * instances, GFP_KERNEL);
    for (i = 0; i < instances; i++) {
        devs.buffers[i] = kzalloc(BUFFER_SIZE, GFP_KERNEL);
    }

    pr_info("----------------------\n");
    pr_info("My module is loaded\n");

    return ret;
}

static void __exit skeleton_exit(void) {
    pr_info("My module unloading...\n");
    pr_info("----------------------\n");

    cdev_del(&devs.cdev);
    unregister_chrdev_region(devs.dev_t, instances);

    int i;
    for(i=0; i < instances; i++) {
        kfree(devs.buffers[i]);
    }
    kfree(devs.buffers);

    pr_info("----------------------\n");
    pr_info("My module is unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR("Fastium <fastium.pro@proton.me>");
MODULE_AUTHOR("Klagarge <remi@heredero.ch>");
MODULE_DESCRIPTION ("Module pilot charachter oriented");
MODULE_LICENSE ("GPL");

/*
 * MAJOR can be find: cat /proc/device
 *
 * For testing with echo and cat:
 *      - mknod /dev/test-device -c 42 0 (Create a charachter device file with the right Major and Minor)
 *      - echo "lalalalalaalalalalallala" > /dev/test-device
 *      - cat /dev/test-device
 *
 */
