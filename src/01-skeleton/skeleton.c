// skeleton.c
#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/moduleparam.h>	// needed for module parameters

#include <linux/slab.h> // dynamic memory allocation
#include <linux/list.h> // linked list
#include <linux/string.h>

#include <linux/ioport.h>
#include <linux/io.h>


#define TEXT_LENGTH_MAX 255


static char* text = "dummy text";
module_param(text, charp, 0664);
static int elements = 1;
module_param(elements, int, 0);


// Ex04 - Dynamic memory allocation and linked list
struct element {
    char text[TEXT_LENGTH_MAX];
    int32_t unique_number;
    struct list_head node;
};

static LIST_HEAD (list_unique_elements);


// Ex05 - Memory-mapped I/O
static struct resource* resources[3] = {[0] = 0,};


static int __init skeleton_init(void) {
	pr_info("Linux module skeleton ex05 loaded\n");
	pr_debug("  text: %s\n  elements: %d\n", text, elements);
	

    // Ex04 - Dynamic memory allocation and linked list
	struct element* element_ptr = kcalloc(elements, sizeof(struct element), GFP_KERNEL);
	if (element_ptr == 0) {
		pr_err("Failed to allocate memory for %d elements\n", elements);
		return -ENOMEM;
	}
	
	uint8_t i;
	const uint8_t length = TEXT_LENGTH_MAX - 1;
	for (i = 0; i < elements; i++) {
		struct element* e = element_ptr + i;
        if (e != 0) {
            strncpy(e->text, text, length);
            e->unique_number = i;
            list_add_tail(&e->node, &list_unique_elements);
			pr_info ("add element %d: %s\n", e->unique_number, e->text);
        }
    }

    // Ex05 - Memory-mapped I/O
    unsigned char* registers[3] = {[0] = 0,};
    uint32_t chipid[4] = {[0] = 0,};
    uint32_t temperature = 0;
    uint32_t mac_address[2] = {[0] = 0,};

    resources[0] = request_mem_region(0x01c14000, 0x1000, "nanopi - chip ID");
    resources[1] = request_mem_region(0x01C25000, 0x1000, "nanopi - temperature sensor");
    resources[2] = request_mem_region(0x01C30000, 0x1000, "nanopi - Ethernet controller");
    if ((resources[0] == 0) || (resources[1] == 0) || (resources[2] == 0)) {
        pr_err("Failed to reserve memory region for chip ID, temperature sensor or Ethernet controller\n");
        return -EFAULT;
    }

    registers[0] = ioremap(0x01c14000, 0x1000);
    registers[1] = ioremap(0x01C25000, 0x1000);
    registers[2] = ioremap(0x01C30000, 0x1000);
    if (registers[0] == 0) {
        pr_err("Failed to map processor registers for chip ID\n");
        return -EFAULT;
    }
     if (registers[1] == 0) {
        pr_err("Failed to map processor registers for temperature sensor\n");
        return -EFAULT;
    }
     if (registers[2] == 0) {
        pr_err("Failed to map processor registers for Ethernet controller\n");
        return -EFAULT;
    }

    chipid[0] = ioread32(registers[0] + 0x200);
    chipid[1] = ioread32(registers[0] + 0x204);
    chipid[2] = ioread32(registers[0] + 0x208);
    chipid[3] = ioread32(registers[0] + 0x20c);
    pr_info(
        "chipid=%08x'%08x'%08x'%08x\n",
        chipid[0], chipid[1], chipid[2], chipid[3]
    );

    temperature = -1991 * (int32_t) ioread32(registers[1] + 0x80) / 10 + 223000;
    pr_info(
        "temperature=%d (register value: %d)\n",
        temperature, ioread32(registers[1] + 0x80)
    );

    mac_address[0] = ioread32(registers[2] + 0x50);
    mac_address[1] = ioread32(registers[2] + 0x54);
    pr_info(
        "mac-addr=%02x:%02x:%02x:%02x:%02x:%02x\n",
        (mac_address[1] >> 0) & 0xff,
        (mac_address[1] >> 8) & 0xff,
        (mac_address[1] >> 16) & 0xff,
        (mac_address[1] >> 24) & 0xff,
        (mac_address[0] >> 0) & 0xff,
        (mac_address[0] >> 8) & 0xff
    );

    iounmap(registers[0]);
    iounmap(registers[1]);
    iounmap(registers[2]);


	return 0;
}

static void __exit skeleton_exit(void) {

    // Ex04 - Dynamic memory allocation and linked list
	struct element* e;

    while (!list_empty(&list_unique_elements)) {
        e = list_entry(list_unique_elements.next, struct element, node);
		pr_info ("delete element %d: %s\n", e->unique_number, e->text);
        list_del(&e->node);
        kfree(e);
    }

    // Ex05 - Memory-mapped I/O
    if (resources[0] != 0) release_mem_region(0x01c14000, 0x1000);
    if (resources[1] != 0) release_mem_region(0x01C25000, 0x1000);
    if (resources[2] != 0) release_mem_region(0x01C30000, 0x1000);

    pr_info ("Linux module skeleton unloaded\n");
}

module_init(skeleton_init);
module_exit(skeleton_exit);

MODULE_AUTHOR("Fastium <fastium.pro@proton.me>");
MODULE_AUTHOR("Klagarge <remi@heredero.ch>");
MODULE_DESCRIPTION("Module skeleton");
MODULE_LICENSE("GPL");
