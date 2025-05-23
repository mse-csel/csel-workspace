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
 * Abstract: System programming - multi-process - ex1
 *
 * Purpose: Safe sleep functions
 *
 * Autĥor:  Vincent Audergon, Bastien Veuthey
 * Date:    2025-05-23
 */

#include "safesleep.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

void safe_sleep_ms(int ms) {
    struct timespec sleep_time = {.tv_nsec = ms * 1000000, };
    struct timespec remaining  = {.tv_nsec=0,};

    while(1) {
        int status = nanosleep (&sleep_time, &remaining);
        if(status == 0) break;
        if (errno == EINTR) {
            sleep_time = remaining;
        } else {
            perror ("nanosleep");
            exit(status);
        }
    }
}

void safe_sleep_s(int s) {
    struct timespec sleep_time = {.tv_sec = s, .tv_nsec = 0, };
    struct timespec remaining  = {.tv_nsec=0,};

    while(1) {
        int status = nanosleep (&sleep_time, &remaining);
        if(status == 0) break;
        if (errno == EINTR) {
            sleep_time = remaining;
        } else {
            perror ("nanosleep");
            exit(status);
        }
    }
}