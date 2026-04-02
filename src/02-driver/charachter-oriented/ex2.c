#include <linux/fs.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/minmax.h>
#include <stddef.h>
#include <linux/uaccess.h>

// linux theory: https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html

#define MY_MAJOR       42
#define MY_MAX_MINORS  5

#define BUFFER_SIZE 300


struct my_device_data {
    struct cdev cdev;
    /* my data starts here */
    char buffer[BUFFER_SIZE];
};

struct my_device_data devs;

// inode: https://www.kernel.org/doc/html/latest/filesystems/ext4/inodes.html
// file: https://docs.kernel.org/filesystems/api-summary.html#c.file

int skeleton_open(struct inode* i, struct file* f) {

    pr_info("Open file\n");

    // Get the stuct of my data
    struct my_device_data *my_data = container_of(i->i_cdev, struct my_device_data, cdev);

    // point private data on driver data, here this is a char buffer
    f->private_data = my_data;

    return 0;
}

int skeleton_release(struct inode* i, struct file* f) {

    pr_info("Release file\n");

    return 0;
}

ssize_t skeleton_read(struct file* f, char* __user buf, size_t count, loff_t* off) {

    pr_info("Read file\n");

    struct my_device_data *my_data = (struct my_device_data *) f->private_data;
    ssize_t len = min(BUFFER_SIZE - ((size_t)*off), count);

    if (len <= 0) {
        pr_info("Cannot read data for length: %ld\n", len);
        return 0;
    }


    /* read data from my_data->buffer to user buffer */
    if (copy_to_user(buf, my_data->buffer + *off, len)) {
        pr_info("Failed to copy to user space buffer\n");
        return -EFAULT;
    }

    *off += len;
    return len;

}

ssize_t skeleton_write(struct file* f, const char* __user buf, size_t count, loff_t* off) {

    pr_info("Write file\n");

    struct my_device_data *my_data = (struct my_device_data *) f->private_data;
    ssize_t len = min(BUFFER_SIZE - ((size_t)*off), count);

    if (len <= 0) {
        pr_info("Cannot write data for length: %ld\n", len);
        return 0;
    }


    /* read data from user buffer to my_data->buffer */
    if (copy_from_user(my_data->buffer + *off, buf, len)) {
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


int ex2_init(void) {
    pr_info("Load exercice 2\n");
    int ret = register_chrdev_region(MKDEV(MY_MAJOR, 0), MY_MAX_MINORS, "My module");

    if (ret != 0) {
        /* report error */
        pr_info("Module registration error: %d\n", ret);
        return ret;
    }

    /* initialize devs fields */
    cdev_init(&devs.cdev, &skeleton_fops);
    ret = cdev_add(&devs.cdev, MKDEV(MY_MAJOR, 0), 1);

    if (ret != 0) {
        /* report error */
        pr_info("cdev add error: %d\n", ret);
        return ret;
    }

    return ret;
}

void ex2_exit(void) {

    cdev_del(&devs.cdev);
    unregister_chrdev_region(MKDEV(MY_MAJOR, 0), MY_MAX_MINORS);
}
