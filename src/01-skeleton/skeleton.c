// skeleton.c
#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/moduleparam.h>	// needed for module parameters

#include <linux/slab.h>
#include <linux/list.h>
#include <linux/string.h>

#define TEXT_LENGTH_MAX 255

static char* text = "dummy text";
module_param(text, charp, 0664);
static int elements = 1;
module_param(elements, int, 0);

struct element {
    char text[TEXT_LENGTH_MAX];
    int unique_number;
    struct list_head node;
};

static LIST_HEAD (list_unique_elements);

static int __init skeleton_init(void) {
	pr_info("Linux module skeleton ex04 loaded\n");
	pr_debug("  text: %s\n  elements: %d\n", text, elements);
	
	struct element* element_ptr = kcalloc(elements, sizeof(struct element), GFP_KERNEL);
	if (element_ptr == 0) {
		pr_err("Failed to allocate memory for %d elements\n", elements);
		return -ENOMEM;
	}
	
	int i;
	const int length = TEXT_LENGTH_MAX - 1;
	for (i = 0; i < elements; i++) {
		struct element* e = element_ptr + i;
        if (e != 0) {
            strncpy(e->text, text, length);
            e->unique_number = i;
            list_add_tail(&e->node, &list_unique_elements);
			pr_info ("add element %d: %s\n", e->unique_number, e->text);
        }
    }
	return 0;
}

static void __exit skeleton_exit(void) {
	struct element* e;

    while (!list_empty(&list_unique_elements)) {
        e = list_entry(list_unique_elements.next, struct element, node);
		pr_info ("delete element %d: %s\n", e->unique_number, e->text);
        list_del(&e->node);
        kfree(e);
    }
    pr_info ("Linux module skeleton unloaded\n");
}

module_init(skeleton_init);
module_exit(skeleton_exit);

MODULE_AUTHOR("Fastium <fastium.pro@proton.me>");
MODULE_AUTHOR("Klagarge <remi@heredero.ch>");
MODULE_DESCRIPTION("Module skeleton");
MODULE_LICENSE("GPL");
