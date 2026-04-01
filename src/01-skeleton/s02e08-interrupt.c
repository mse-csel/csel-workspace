#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/interrupt.h>
#include <linux/gpio.h>
#include "linux/printk.h"

struct gpio_nanopi {
    int id;
    char* name;
};

static struct gpio_nanopi switchK1 = {0, "K1: GPIOA.0"};
static struct gpio_nanopi switchK2 = {2, "K2: GPIOA.2"};
static struct gpio_nanopi switchK3 = {3, "K3: GPIOA.3"};

irqreturn_t isrGPIO(int irq, void* gpio_struct) {
    struct gpio_nanopi *gpio = (struct gpio_nanopi *)gpio_struct;
    pr_info("GPIO pressed: %s\n", gpio->name);
    return IRQ_HANDLED;
}


void interrupt_init(void) {
    pr_info("Initializing interrupts\n");

    int status = 0;

    // Switch k1
    if (gpio_request(switchK1.id, switchK1.name) == 0) {
        status = request_irq(
            gpio_to_irq(switchK1.id),
            isrGPIO,
            IRQF_TRIGGER_FALLING | IRQF_SHARED,
            switchK1.name,
            &switchK1
        );
    }

    // Switch k2
    if (gpio_request(switchK2.id, switchK2.name) == 0) {
        status = request_irq(
            gpio_to_irq(switchK2.id),
            isrGPIO,
            IRQF_TRIGGER_FALLING | IRQF_SHARED,
            switchK2.name,
            &switchK2
        );
    }

    // Switch k3
    if (gpio_request(switchK3.id, switchK3.name) == 0) {
        status = request_irq(
            gpio_to_irq(switchK3.id),
            isrGPIO,
            IRQF_TRIGGER_FALLING | IRQF_SHARED,
            switchK3.name,
            &switchK3
        );
    }

    pr_info("Interrupt initialized\n");
}

void interrupt_exit(void) {
    pr_info("Exiting interrupts\n");

    gpio_free(switchK1.id);
    free_irq(gpio_to_irq(switchK1.id), &switchK1);

    gpio_free(switchK2.id);
    free_irq(gpio_to_irq(switchK2.id), &switchK2);

    gpio_free(switchK3.id);
    free_irq(gpio_to_irq(switchK3.id), &switchK3);

    pr_info ("Interrupt exited\n");

}
