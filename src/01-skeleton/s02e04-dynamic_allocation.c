#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/slab.h> // dynamic memory allocation
#include <linux/list.h> // linked list
#include <linux/string.h>

#define TEXT_LENGTH_MAX 255

struct element {
    char text[TEXT_LENGTH_MAX];
    int32_t unique_number;
    struct list_head node;
};

static LIST_HEAD (list_unique_elements);

void dynAlloc_init(void) {
    pr_info("Initialize dynamic allocation and linked list\n");
    
    struct element* element_ptr = kcalloc(elements, sizeof(struct element), GFP_KERNEL);
	if (element_ptr == 0) {
		pr_err("Failed to allocate memory for %d elements\n", elements);
		return;
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

    pr_info("Dynamic allocation and linked list initialized\n");
}

void dynAlloc_exit(void) {
    pr_info("Free memory allocated for dynamic allocation and linked list\n");

    struct element* e;

    while (!list_empty(&list_unique_elements)) {
        e = list_entry(list_unique_elements.next, struct element, node);
        pr_info ("delete element %d: %s\n", e->unique_number, e->text);
        list_del(&e->node);
        if (e != 0) {
            kfree(e);
        }
    }

    pr_info("Memory allocated for dynamic allocation and linked list freed\n");
}