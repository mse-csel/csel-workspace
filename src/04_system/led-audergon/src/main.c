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
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <time.h>
#include "led.h"
#define LED "10"

/*
 * status led - gpioa.10 --> gpio10
 * power led  - gpiol.10 --> gpio362
 */


int main(int argc, char* argv[])
{
    long duty   = 2;     // %
    long period = 1000;  // ms
    if (argc >= 2) period = atoi(argv[1]);
    period *= 1000000;  // in ns

    // compute duty period...
    long p1 = period / 100 * duty;
    long p2 = period - p1;

    int led = open_led(LED);
    led_toggle(led, true);


    struct itimerspec spec;
    memset(&spec, 0, sizeof(spec));
    spec.it_interval.tv_sec = 5;
    spec.it_value.tv_sec = 5;

    int timerfd = timerfd_create(CLOCK_REALTIME, 0);
    if (timerfd == -1) {
        perror("ERROR");
    }
    if (timerfd_settime(timerfd, 0, &spec, NULL) == -1) {
        perror("ERROR");
    }

    int epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = timerfd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, timerfd, &ev) == -1) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    bool led_state = false;

    while (1) {
        struct epoll_event events[1];
        int nfds = epoll_wait(epfd, events, 1, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        // On lit le timerfd pour réarmer le timer
        uint64_t expirations;
        read(timerfd, &expirations, sizeof(expirations));
        led_toggle(led, led_state);
        led_state = !led_state;
        
    }
    
    return 0;
}