#import "/doc/metadata.typ": *

=== Interrupts  <lab01:ex08>
#colorbox(title: "Exercise", color: hei-blue)[
  Develop a module which allows to detect every push on the button of the nanopi with interrupt. Every interrupts will send a message for debugging.

  - Use the service ``` gpio_request(<io_nr>, <label>)```
  - Get the interrupt vector with ``` gpio_to_irq(<io_nr>)```
  - Extension card information:
    - k1 - gpio: A, pin_nr=0, io_nr=0
    - k2 - gpio: A, pin_nr=2, io_nr=2
    - k3 - gpio: A, pin_nr=3, io_nr=3
]

We made a custom structur for the gpio device that contain all useful information like the name and the id.
```c
struct gpio_nanopi {
    int id;
    char* name;
};
static struct gpio_nanopi switchK1 = {0, "K1: GPIOA.0"};
static struct gpio_nanopi switchK2 = {2, "K2: GPIOA.2"};
static struct gpio_nanopi switchK3 = {3, "K3: GPIOA.3"};
```
