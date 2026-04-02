#import "/doc/metadata.typ": *

#task(
  [
    Create module with dynamic allocation and a chained list
  ],
  [
    Create dynamically elements in the kernel. Adapt a kernel module to specify at the installation the number of element to create a initial text.
    Each element will contain a unique number. The elements are create at the installation of the module adn chained in a list.
    These elements will be destruct during the uninstallation of the module.
    Some information messages are emits to allow debugging.
  ]
)

To allocate memory in the kernel, we can use the `kcalloc` function. It allows to allocate directly the memory for all element. It's also possible to use `kzalloc` in a loop to allocate memory for each element. We prefer allocate all the memory at once to avoid fragmentation and to be sure all the memory can be allocated.

```bash
struct element* element_ptr = kcalloc(elements, sizeof(struct element), GFP_KERNEL);

for (int i = 0; i < elements; i++) {
  struct element* e = element_ptr + i;
      if (e != 0) {
          strncpy(e->text, text, TEXT_LENGTH_MAX - 1);
          e->unique_number = i;
          list_add_tail(&e->node, &list_unique_elements);
    pr_info ("add element %d: %s\n", e->unique_number, e->text);
      }
  }
```
