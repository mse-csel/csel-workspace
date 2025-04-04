// skeleton.c
#include <linux/module.h>       // needed by all modules
#include <linux/init.h>         // macros
#include <linux/kernel.h>       // debugging and vsnprintf
#include <linux/moduleparam.h>  // module/driver parameters
#include <linux/cdev.h>         // char driver
#include <linux/fs.h>           // driver
#include <linux/uaccess.h>      // copy data to/from the user
#include <linux/kthread.h>      // thread
#include <linux/delay.h>        // delay [temporary]

//--- arguments
static char* prime_compute_file = "/opt/.prime_computation";
module_param(prime_compute_file, charp, 0);

static char* prime_list = "/opt/prime";
module_param(prime_list, charp, 0);

//--- global values

#define             BUFFER_MAX_SZ 1000
static const char*  READ_FORMAT = "current computation state : \
    p= %d, current t = %d, current n = %d, current computation time %llu\n";
static const char*  SAVE_FORMAT = "%d %d %d %d";   //p, current t, current n, next_is_add, current computation time
static const char*  REPORT_FORMAT = "%d: %d is_opti : %d\n"; //p: t - computation time
static struct task_struct* compute_thread;

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

//--- m_dec functions and values

static int current_p = 0;
static int current_t = 0;
static int current_n = 0;
static int next_is_add = false;
static int currently_processing = true;
static int current_is_optimal;

static void retrieve_computation_state(void){
    struct file* file;
    ssize_t len;
    char data[BUFFER_MAX_SZ];
    file = filp_open(prime_compute_file, O_RDWR | O_CREAT, 0644);
    if(file){   //file properly open
        memset(data, 0, sizeof(data));
        file->f_pos=0;
        len = kernel_read(file, data, sizeof(data), &file->f_pos);
        len = sscanf(data, SAVE_FORMAT, &current_p, &current_t, &current_n, &next_is_add);
        if(0 == len){
            current_p = 3;
            current_t = 1;
            current_n = 1;
            next_is_add = false;
            currently_processing = false;
        }else{}
        filp_close(file, NULL);
    }else{}
    return;
}

static void save_computation_state(void){
    struct file* file;
    ssize_t len;
    char data[BUFFER_MAX_SZ];
    file = filp_open(prime_compute_file, O_RDWR | O_CREAT, 0644);
    if(file){   //file properly open
        len = snprintf(data, BUFFER_MAX_SZ, SAVE_FORMAT, current_p, current_t, current_n, next_is_add);
        len = kernel_write(file, data, len, &file->f_pos);
        filp_close(file, NULL);
    }else{}
    return;
}

static void report_computation(void){
    struct file* file;
    ssize_t len;
    char data[BUFFER_MAX_SZ];
    file = filp_open(prime_list, O_RDWR | O_APPEND | O_CREAT, 0644);
    if(file){
        len = snprintf(data, BUFFER_MAX_SZ, REPORT_FORMAT, current_p, current_t, current_is_optimal);
        len = kernel_write(file, data, len, &file->f_pos);
        filp_close(file, NULL);
    }else{}
    return;
}

void execute_computation(void){
    int minus_p = -current_p;
    if(!currently_processing){
        current_n = (current_p/2)+1;
    }else{
        currently_processing = false;
    }
    while((current_n != 1 && current_n != -1) && !kthread_should_stop()){
        if((current_n&0x1) == 0x1){
            current_n += (next_is_add ? current_p : minus_p);
            next_is_add = !next_is_add;
        }else{}
        current_n >>= 1;
        current_t += 1;
    }
    if(!kthread_should_stop()){
        current_is_optimal = (current_n == -1);
        current_t *= (current_is_optimal ? 2 : 1);
    }else{}
    return;
}

static int thread_func(void* data){
    pr_info("starting thread");
    while(!kthread_should_stop()){
        execute_computation();
        if(!kthread_should_stop()){
            report_computation();
            current_p += 2;
            current_t = 1;
        }else{}
    }
    save_computation_state();
    return 0;
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

    nb_char = snprintf(
        tmp_str, 
        BUFFER_MAX_SZ, 
        READ_FORMAT, 
        current_p, 
        current_t, 
        current_n, 
        (unsigned long long)0x142857
    );

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
        retrieve_computation_state();
        compute_thread = kthread_run(thread_func, 0, "s/thread");
    }else{}
    return 0;
}

// exit module (rmmod, ...)
static void __exit skeleton_exit(void){
    kthread_stop(compute_thread);
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
