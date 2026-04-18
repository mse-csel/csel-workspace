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

/*
 * status led - gpioa.10 --> gpio10
 * power led  - gpiol.10 --> gpio362
 */
#define GPIO_EXPORT   "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"
#define GPIO_LED      "/sys/class/gpio/gpio10"
#define LED           "10"

#define GPIO_BTN1      "/sys/class/gpio/gpio0"
#define BTN1          "0"
#define GPIO_BTN2      "/sys/class/gpio/gpio2"
#define BTN2          "2"
#define GPIO_BTN3      "/sys/class/gpio/gpio3"
#define BTN3          "3"


static int open_led(char* gpio_path, char* pin)
{

    // unexport pin out of sysfs (reinitialization)
    int f = open(GPIO_UNEXPORT, O_WRONLY);
    write(f, pin, strlen(pin));
    close(f);

    // export pin to sysfs
    f = open(GPIO_EXPORT, O_WRONLY);
    write(f, pin, strlen(pin));
    close(f);

    // config pin
    char direction_path[100];
    strcpy(direction_path, gpio_path);
    strcat(direction_path, "/direction");

    f = open(direction_path, O_WRONLY);
    write(f, "out", 3);
    close(f);

    // open gpio value attribute
    char value_path[100];
    strcpy(value_path, gpio_path);
    strcat(value_path, "/value");

    f = open(value_path, O_RDWR);
    return f;
}

static int open_btn(char* gpio_path, char* pin) {
    int f = open(GPIO_UNEXPORT, O_WRONLY);
    write(f, pin, strlen(pin));
    close(f);

    f = open(GPIO_EXPORT, O_WRONLY);
    write(f, pin, strlen(pin));
    close(f);

    char direction_path[100];
    strcpy(direction_path, gpio_path);
    strcat(direction_path, "/direction");

    f = open(direction_path, O_WRONLY);
    write(f, "in", 2);
    close(f);

    char edge_path[100];
    strcpy(edge_path, gpio_path);
    strcat(edge_path, "/edge");
    f = open(edge_path, O_WRONLY);
    write(f, "both", 4); close(f);

    char value_path[100];
    strcpy(value_path, gpio_path);
    strcat(value_path, "/value");

    f = open(value_path, O_RDONLY);
    return f;
}

int main(int argc, char* argv[])
{
    long duty   = 2;     // %
    long period = 1000;  // ms
    if (argc >= 2) period = atoi(argv[1]);
    period *= 1000000;  // in ns

    // compute duty period...
    long p1 = period / 100 * duty;
    long p2 = period - p1;

    int led = open_led(GPIO_LED, LED);
    pwrite(led, "1", sizeof("1"), 0);

    int btn1 = open_btn(GPIO_BTN1, BTN1);
    int btn2 = open_btn(GPIO_BTN2, BTN2);
    int btn3 = open_btn(GPIO_BTN3, BTN3);

    // epoll
    int epfd = epoll_create1(0);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = btn1;
    epoll_ctl(epfd, EPOLL_CTL_ADD, btn1, &ev);
    ev.data.fd = btn2;
    epoll_ctl(epfd, EPOLL_CTL_ADD, btn2, &ev);
    ev.data.fd = btn3;
    epoll_ctl(epfd, EPOLL_CTL_ADD, btn3, &ev);


    struct timespec t1;
    clock_gettime(CLOCK_MONOTONIC, &t1);

    int k = 0;
    while (1) {
        struct timespec t2;
        clock_gettime(CLOCK_MONOTONIC, &t2);

        long delta =
            (t2.tv_sec - t1.tv_sec) * 1000000000 + (t2.tv_nsec - t1.tv_nsec);

        int toggle = ((k == 0) && (delta >= p1)) | ((k == 1) && (delta >= p2));
        if (toggle) {
            t1 = t2;
            k  = (k + 1) % 2;
            if (k == 0)
                pwrite(led, "1", sizeof("1"), 0);
            else
                pwrite(led, "0", sizeof("0"), 0);
        }

        struct epoll_event events[3];
        int n = epoll_wait(epfd, events, 3, 0);
        // printf("epoll_wait returned %d events\n", n);
        char buf[2];
        for (int i = 0; i < n; i++) {
            // printf ("event=%d on fd=%d\n", events[i].events, events[i].data.fd);
            if (events[i].data.fd == btn1) {
                pread(btn1, buf, sizeof(buf), 0);
                if (buf[0] == '1') {
                    printf("btn1 activated\n");
                }
            } else if (events[i].data.fd == btn2) {
                // printf("btn2 pressed\n");
            } else if (events[i].data.fd == btn3) {
                // printf("btn3 pressed\n");
            }
        }

    }

    return 0;
}
