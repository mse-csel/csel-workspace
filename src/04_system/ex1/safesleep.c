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