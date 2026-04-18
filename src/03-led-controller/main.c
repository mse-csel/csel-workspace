/**
 * Copyright 2018 University of Applied Sciences Western Switzerland / Fribourg
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
 * Purpose: NanoPi silly status led control system
 *
 * Autĥor:  Daniel Gachet
 * Date:    07.11.2018
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/inotify.h>

#include "setup.c"

/*
 * status led - gpioa.10 --> gpio10
 * power led  - gpiol.10 --> gpio362
 */

#define GPIO_LED      "/sys/class/gpio/gpio10"
#define LED           "10"

#define GPIO_BTN1      "/sys/class/gpio/gpio0"
#define BTN1          "0"
#define GPIO_BTN2      "/sys/class/gpio/gpio2"
#define BTN2          "2"
#define GPIO_BTN3      "/sys/class/gpio/gpio3"
#define BTN3          "3"




int main(void)
{
    printf("Starting Button 1 Test...\n");

    int btn1 = open_btn(GPIO_BTN1, BTN1);
    if (btn1 < 0) {
        perror("Failed to open button");
        return 1;
    }

    // 1. Create epoll instance
    int epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("Failed to create epoll");
        return 1;
    }

    // 2. Add button to epoll
    struct epoll_event ev;
    // CRITICAL: Sysfs GPIOs use EPOLLPRI (priority data) and EPOLLERR, not EPOLLIN!
    ev.events = EPOLLPRI | EPOLLERR | EPOLLET;
    ev.data.fd = btn1;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, btn1, &ev) < 0) {
        perror("Failed to add btn1 to epoll");
        return 1;
    }

    // 3. Dummy read to clear initial state before waiting
    char buf[2];
    pread(btn1, buf, sizeof(buf), 0);

    printf("Waiting for button presses (CPU should be at 0%%)...\n");

    // 4. Event loop
    while (1) {
        struct epoll_event events[1]; // We only care about 1 event for now

        // Timeout is -1: Block infinitely until an event occurs!
        int n = epoll_wait(epfd, events, 1, -1);

        if (n < 0) {
            perror("epoll_wait error");
            break;
        }

        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == btn1) {
                // Read the new value. pread uses offset 0 so we don't need lseek()
                pread(btn1, buf, sizeof(buf), 0);

                // Print the result. '1' or '0' depends on your hardware pull-up/down resistors
                if (buf[0] == '1') {
                    printf("Button 1 State: HIGH (1)\n");
                } else {
                    printf("Button 1 State: LOW (0)\n");
                }
            }
        }
    }

    close(btn1);
    close(epfd);
    return 0;
}
