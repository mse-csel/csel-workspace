#ifndef LED_H_
#define LED_H_

#include <stdbool.h>

/**
 * Opens a led in the sysfs and returns its corresponding file descriptor.
 * 
 * @return the file descriptor or -1 if an error occured.
 */
int open_led();

/**
 * Changes the status of a led
 * 
 * @param fd the file descritor corresponding to the led
 * @param bool the new status of the led (on = true, off = false)
 */
void led_toggle(int fd, bool on);

#endif