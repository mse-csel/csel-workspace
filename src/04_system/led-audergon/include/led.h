/**
 * Copyright 2025 University of Applied Sciences Western Switzerland / Fribourg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Project: HEIA-FR / HES-SO MSE - MA-CSEL1 Laboratory
 *
 * Abstract: System programming -  file system
 *
 * Autĥor:  Vincent Audergon
 * Date:    11.04.2025
 */

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