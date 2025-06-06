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

//--- global/static values

static const int NOT_FOUND = 0;
static const int IS_FOUND = 1;

//note: below are enum and array linked to them. While this construction is 
//      a bit of tinkering in C (because modification requires extra care
//      to not miss an enum element), it becomes trivial and 
//      straightforward when using map (in C++ for instance)

typedef enum{
    automatic = 0,
    manual,
    state_last, //iteration purpose
}state;
char* const STATE_NAME[] = {
    "automatic",
    "manual"
};
static state current_state = automatic;

typedef enum{
    BELOW_35 = 0,
    BELOW_40,
    BELOW_45,
    ABOVE_45,
    temp_range_last,    //iteration purpose
}temp_class;
static temp_class current_temp_class = BELOW_35;
static int FREQUENCY[] = {
    2, 5, 10, 20
};

typedef enum{
    lower = 0,
    higher,
    manual_input_last,  //iteration_purpose
}manual_input;
char* const MANUAL_INPUT_NAME[] = {
    "lower",
    "higher"
};

#define             BUFFER_MAX_SZ 1000
static char tmp_str[BUFFER_MAX_SZ];
static const char*  READ_FORMAT = "%ld, %s, %d\n"; //processor_temp, current_mode, led_frequency
static const long TEMP_ADDR_START = 0x01c25000;
static const int TEMP_ADDR_CONF = 0x1000;

static struct resource* res = 0;
static unsigned char* reg = 0;
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

static void set_current_temp_class(int new_temp_class){
    pr_info("prior %d, new %d\n", current_temp_class, new_temp_class);
    current_temp_class = new_temp_class;
}

static int interpret_change_state(void){
    int ret = NOT_FOUND;
    int input;
    for(input=0;input<state_last;input++){
        if(0 == strcmp(STATE_NAME[input], tmp_str)){
            if(input != current_state){
                current_state = input;
            }else{}
            ret = IS_FOUND;
            break;
        }
    }
    return ret;
}

static int interpret_manual_input(void){
    int ret = NOT_FOUND;
    int input;
    for(input=0;input<manual_input_last;input++){
        if(0 == strcmp(MANUAL_INPUT_NAME[input], tmp_str)){
            switch(input){
                case lower:
                    ret = IS_FOUND;
                    //if allowed to decrease
                    if(current_temp_class != BELOW_35){
                        set_current_temp_class(current_temp_class-1);
                    }else{}
                    break;
                case higher:
                    ret = IS_FOUND;
                    if(current_temp_class != ABOVE_45){
                        set_current_temp_class(current_temp_class+1);
                    }else{}
                    break;
                default:
                    ret = NOT_FOUND;
                    break;
            }
            break;
        }else{}
    }
    return ret;
}

static int interpret_automatic_input(void){
    int ret = NOT_FOUND;
    //no automatic input, leaving the function for future modifications
    return ret;
}

static void interpret_input(void){
    int status = interpret_change_state();
    if(NOT_FOUND == status){
        switch(current_state){
            case automatic:
                status = interpret_automatic_input();
                break;
            case manual:
                status = interpret_manual_input();
                break;
            default:
                break;
        }
    }else{}
    if(NOT_FOUND == status){
        pr_info("unknown argument %s in %s mode\n", tmp_str, STATE_NAME[current_state]);
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
    return FREQUENCY[current_temp_class];
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
    nb_char = snprintf(tmp_str, BUFFER_MAX_SZ, READ_FORMAT, get_temp(), STATE_NAME[current_state], get_frequency());

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
