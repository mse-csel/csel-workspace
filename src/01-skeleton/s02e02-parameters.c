#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/moduleparam.h>

static char* text = "dummy text";
module_param(text, charp, 0664);
static int elements = 1;
module_param(elements, int, 0);

void parameters_print(void) {
    pr_debug("text: %s\n", text);
    pr_debug("elements: %d\n", elements);
}