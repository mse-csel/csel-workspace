// skeleton.c
#include <linux/module.h>       // needed by all modules
#include <linux/init.h>         // macros
#include <linux/kernel.h>       // debugging and vsnprintf
#include <linux/moduleparam.h>  // module/driver parameters
#include <linux/cdev.h>         // char driver
#include <linux/fs.h>           // driver
#include <linux/uaccess.h>      // copy data to/from the user

//--- arguments
static char* prime_compute_file = "/opt/.prime_computation";
module_param(prime_compute_file, charp, 0);

static char* prime_list = "/opt/prime";
module_param(prime_list, charp, 0);

//--- global values

#define             BUFFER_MAX_SZ 1000
static const char*  READ_FORMAT = "current computation state : \
    p= %d, current tmd_2 = %d, current n = %d, current computation time %llu\n";

//--- utils

//from : https://stackoverflow.com/a/12264427
int snprintf(char *buf, size_t size, const char *fmt, ...){
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsnprintf(buf, size, fmt, args);
    va_end(args);

    return i;
}

//--- driver functions

//does nothing
static int skeleton_open(struct inode* i, struct file* f){
    return 0;
}

//does nothing
static int skeleton_release(struct inode* i, struct file* f){
    return 0;
}

static ssize_t skeleton_read(
    struct file* f,
    char __user* buf, 
    size_t count,
    loff_t* off
){
    int nb_char = -EFAULT;
    static char tmp_str[BUFFER_MAX_SZ];

    ssize_t remaining = BUFFER_MAX_SZ - (ssize_t)(*off);                                                                    
    char* ptr         = tmp_str + *off;                                                                                
    if (count > remaining) count = remaining;                                                                           
    *off += count;

    nb_char = snprintf(tmp_str, BUFFER_MAX_SZ, READ_FORMAT, 7, 3, 1, (unsigned long long)0x142857);

    if(0 != copy_to_user(buf, ptr, count)){
        nb_char = -EFAULT;
    }else{}
    return count;
}

static struct file_operations skeleton_fops = {
    .owner = THIS_MODULE,
    .open = skeleton_open,
    .read = skeleton_read,
    .release = skeleton_release,
};

//--- module functions

static dev_t skeleton_dev;
static struct cdev skeleton_cdev;

// init module (modprobe, insmod, ...)
static int __init skeleton_init(void){
    int status = alloc_chrdev_region(&skeleton_dev, 0, 1, "prime_compute");
    pr_info("hello_there\nprime list location : %s\n", prime_list);
    if(0 == status){
        cdev_init(&skeleton_cdev, &skeleton_fops);
        skeleton_cdev.owner = THIS_MODULE;
        status = cdev_add(&skeleton_cdev, skeleton_dev, 1);
    }else{}
    return 0;
}

// exit module (rmmod, ...)
static void __exit skeleton_exit(void){
    cdev_del(&skeleton_cdev);
    unregister_chrdev_region(skeleton_dev, 1);
    pr_info("writing computation state in %s\nHave a nice day\n", prime_compute_file);
    return;
}

module_init(skeleton_init);
module_exit(skeleton_exit);

MODULE_AUTHOR("Adrien Balleyguier <adrien.balleyguier@hefr.ch>");
MODULE_DESCRIPTION("prime computation skeleton");
MODULE_LICENSE("GPL");
