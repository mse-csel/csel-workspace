#include "button.h"

int button_init(button_t* b, int gpio, unsigned int press_time_until_repeat_ms, unsigned int repeat_period_ms) {
    if (gpio_init(&b->gpio_ro, gpio, GPIO_DIR_IN) == -1) {
        error("could not initialize GPIO for button");
        return -1;
    }
    b->press_time_until_repeat_ms = press_time_until_repeat_ms;
    b->repeat_period_ms = repeat_period_ms;
    b->_last_change = time(NULL);
    b->_last_repeat = time(NULL);
    b->_state = false;
    b->_repeat_triggered = false;
    b->_on_click = NULL;
    b->_on_repeat = NULL;
    return 0;
}

void button_attach_on_click(button_t* b, void (*func)(void*), void* param) {
    b->_on_click = func;
    b->_on_click_param = param;
}
void button_attach_on_repeat(button_t* b, void (*func)(void*), void* param) {
    b->_on_repeat = func;
    b->_on_repeat_param = param;
}

void button_update(button_t* b) {
    bool state = gpio_read(&b->gpio_ro);
    time_t now = time(NULL);
    if (state != b->_state) {
        if (state == true) { // rising edge
            if (b->_on_click != NULL) {
                b->_on_click();
            }
        }
        b->_repeat_triggered = false; // reset repeat
    }
    if (b->_state == true && !b->_repeat_triggered) { // still pressed
        if (difftime(now, b->_last_change) > b->press_time_until_repeat_ms) {
            b->_repeat_triggered = true;
            b->_last_repeat = now;
            if (b->_on_repeat != NULL) {
                b->_on_repeat();
            }
        }
    }
    if (b->_repeat_triggered) {
        if (difftime(now, b->_last_repeat) > b->repeat_period_ms) {
            if (b->_on_repeat != NULL) {
                b->_on_repeat();
            }
            b->_last_repeat = now;
        }
    }
    b->_state = state;
    b->_last_change = now;
}