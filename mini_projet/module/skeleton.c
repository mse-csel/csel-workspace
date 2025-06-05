// skeleton.c
#include <linux/module.h>       // needed by all modules
#include <linux/init.h>         // macros
#include <linux/kernel.h>       // debugging and vsnprintf
#include <linux/moduleparam.h>  // module/driver parameters
#include <linux/cdev.h>         // char driver
#include <linux/fs.h>           // driver
#include <linux/uaccess.h>      // copy data to/from the user
#include <linux/ioport.h>	//memory region handling
#include <linux/io.h>		//mmio handling

//--- arguments
static char* prime_compute_file = "/opt/.prime_computation";
module_param(prime_compute_file, charp, 0);

static char* prime_list = "/opt/prime";
module_param(prime_list, charp, 0);

//--- global/static values

typedef enum{
    automatic = 0,
    manual,
    state_last, //iteration purpose
}state;
char* const STATE_NAME[] = {
    "auto",
    "manual"
};
static state current_state = automatic;

typedef enum{
    below_35 = 0,
    below_40,
    below_45,
    higher_45,
    temp_range_last,    //iteration purpose
}temp_range;
static temp_range current_temp_range = below_35;

#define             BUFFER_MAX_SZ 1000
static char tmp_str[BUFFER_MAX_SZ];
static const char*  READ_FORMAT = "%ld, %s, %d\n"; //processor_temp, current_mode, led_frequency
static const long TEMP_ADDR_START = 0x01c25000;
static const int TEMP_ADDR_CONF = 0x1000;

static struct resource* res = 0;
static unsigned char* reg = 0;
static char* current_mode_name = STATE_NAME[automatic];
static long temp = -50l;   //small enough ot be obviously wrong

static dev_t skeleton_dev;
static struct cdev skeleton_cdev;

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

//from : https://stackoverflow.com/q/17559996
int strcmp(const char* s1, const char* s2){
    while(*s1 && (*s1==*s2)){
        s1++;
        s2++;
    }
    return (*(const unsigned char*)s1-*(const unsigned char*)s2);
}

//--- modules dedicated functions and values

static void change_state(state new_state){
    if(new_state != current_state){
        current_state = new_state;
        pr_info("changed state to %s\n", STATE_NAME[current_state]);
    }else{}
    return;
}

static void interpret_input(void){
    int found = 0;
    int i;
    for(i=0;i<state_last;i++){
        if(0 == strcmp(STATE_NAME[i],tmp_str)){
            found = 1;
            change_state(i);
            break;
        }else{}
    }
    if(0 == found){
        pr_info("unknown state : %s\n", tmp_str);
    }else{}
    return;
}

static void update_temp(void){
    if(0 != reg){
        temp = -1191 * (int)ioread32(reg+0x80) / 10 + 223000;
    }else{} 
    return;
}

static long get_temp(void){
    update_temp();
    return temp;
}

static int get_frequency(void){
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

    ssize_t remaining = BUFFER_MAX_SZ - (ssize_t)(*off);                                                                    
    char* ptr         = tmp_str + *off;                                                                                
    if (count > remaining) count = remaining;                                                                           
    *off += count;

    update_temp();
    nb_char = snprintf(tmp_str, BUFFER_MAX_SZ, READ_FORMAT, get_temp(), current_mode_name, get_frequency());

    if(0 != copy_to_user(buf, ptr, nb_char)){
        nb_char = -EFAULT;
    }else{}
    return count;
}

static ssize_t skeleton_write(struct file*f, const char __user* buf, size_t count, loff_t* off){
    ssize_t remaining = BUFFER_MAX_SZ - (ssize_t)(*off);
    if(count >= remaining){
        count = -EIO;
    }else{}
    if(count > 0){
        char* ptr = tmp_str + *off;
        *off += count;
        ptr[count] = 0; //ensures that last char is \0
        if(copy_from_user(ptr, buf, count)){
            count = -EFAULT;
        }else{} 
        //removing trailing newline if any
        if(ptr[count-1] == '\n'){
            ptr[count-1] = '\0';
        }else{}
        interpret_input();
    }else{}
    return count;
}

static struct file_operations skeleton_fops = {
    .owner = THIS_MODULE,
    .open = skeleton_open,
    .read = skeleton_read,
    .write = skeleton_write,
    .release = skeleton_release,
};

//--- module functions

// init module (modprobe, insmod, ...)
static int __init skeleton_init(void){
    int status;
    pr_info("hello there, mini_project module loaded\n");
    res = request_mem_region(TEMP_ADDR_START, TEMP_ADDR_CONF, "allwinner h5 ths");
    if(0 == res){
        pr_info("error while reserving memory region\n");
    }else{}
    reg = ioremap(TEMP_ADDR_START, TEMP_ADDR_CONF);
    if(0 == reg){
        pr_info("error while trying to map processor register\n");
    }else{}

    status = alloc_chrdev_region(&skeleton_dev, 0, 1, "mini_project");
    if(0 == status){
        cdev_init(&skeleton_cdev, &skeleton_fops);
        skeleton_cdev.owner = THIS_MODULE;
        status = cdev_add(&skeleton_cdev, skeleton_dev, 1);
    }else{}
    return 0;
}

// exit module (rmmod, ...)
static void __exit skeleton_exit(void){
    if(0 != res){
        release_mem_region(TEMP_ADDR_START, TEMP_ADDR_CONF);
    }else{}
    cdev_del(&skeleton_cdev);
    unregister_chrdev_region(skeleton_dev, 1);
    pr_info("have a nice day\n");
    return;
}

module_init(skeleton_init);
module_exit(skeleton_exit);

MODULE_AUTHOR("Adrien Balleyguier <adrien.balleyguier@hefr.ch>");
MODULE_DESCRIPTION("prime computation skeleton");
MODULE_LICENSE("GPL");
