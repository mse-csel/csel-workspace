#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/interrupt.h>
#include <linux/gpio.h>
#include "linux/printk.h"

#define SWITCH_K1 0
#define SWITCH_K2 2
#define SWITCH_K3 3

struct gpio_nanopi {
    int pin;
    char* name;
};
static struct gpio_nanopi switchK1 = {0, "K1: GPIOA.0"};
static struct gpio_nanopi switchK2 = {0, "K2: GPIOA.2"};
static struct gpio_nanopi switchK3 = {0, "K3: GPIOA.3"};

irqreturn_t isrGPIO(int irq, void* gpio_struct) {
    struct gpio_nanopi *gpio = (struct gpio_nanopi *)gpio_struct;
    pr_info("GPIO pressed: %s\n", gpio->name);
    return IRQ_HANDLED;
}


void interrupt_init(void) {
    pr_info("Initializing interrupts\n");

    int status = 0;

    // Switch k1
    if (gpio_request(SWITCH_K1, switchK1.name) == 0) {
        status = request_irq(
            gpio_to_irq(SWITCH_K1),
            isrGPIO,
            IRQF_TRIGGER_FALLING | IRQF_SHARED,
            switchK1.name,
            &switchK1
        );
    }

    // Switch k2
    if (gpio_request(SWITCH_K2, switchK2.name) == 0) {
        status = request_irq(
            gpio_to_irq(SWITCH_K2),
            isrGPIO,
            IRQF_TRIGGER_FALLING | IRQF_SHARED,
            switchK2.name,
            &switchK2
        );
    }

    // Switch k3
    if (gpio_request(SWITCH_K3, switchK3.name) == 0) {
        status = request_irq(
            gpio_to_irq(SWITCH_K3),
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

    gpio_free(SWITCH_K1);
    free_irq(gpio_to_irq(SWITCH_K1), &switchK1);

    gpio_free(SWITCH_K2);
    free_irq(gpio_to_irq(SWITCH_K2), &switchK2);

    gpio_free(SWITCH_K3);
    free_irq(gpio_to_irq(SWITCH_K3), &switchK3);

    pr_info ("Interrupt exited\n");

}
