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
#include <pthread.h>

#include "setup.c"

/*
 * status led - gpioa.10 --> gpio10
 * power led  - gpiol.10 --> gpio362
 */

#define GPIO_LED       "/sys/class/gpio/gpio10"
#define LED            "10"

#define GPIO_BTN1      "/sys/class/gpio/gpio0"
#define BTN1           "0"
#define GPIO_BTN2      "/sys/class/gpio/gpio2"
#define BTN2           "2"
#define GPIO_BTN3      "/sys/class/gpio/gpio3"
#define BTN3           "3"

#define NBR_BTN 3

// constant
const char* GPIO_BTN[NBR_BTN] = {GPIO_BTN1, GPIO_BTN2, GPIO_BTN3};
const char* BTN[NBR_BTN] =  {BTN1, BTN2, BTN3};

void* btn_thread(void* arg) {
    // Open all button with the right flags
    int btn[NBR_BTN] = {0};
    for(int i=0; i<NBR_BTN; i++) {
        btn[i] = open_btn(GPIO_BTN[i], BTN[i]);
        if (btn[i] < 0) {
            perror("Failed to open button");
        }
    }

    // Create epoll instance to control all button files
    int epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("Failed to create epoll");
    }

    // Add buttons to epoll
    struct epoll_event ev[NBR_BTN];
    // EPOLLIN is working well as EPOLLPRI (which is more used for priority data)
    // EPOLLERR is used to detect if there is an error
    // EPOLLET is for edge triggered mode (non-blocking)
    for(int i=0; i<NBR_BTN; i++) {
        ev[i].events = EPOLLIN | EPOLLERR | EPOLLET;
        ev[i].data.fd = btn[i];
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, btn[i], &ev[i]) < 0) {
            perror("Failed to add button to epoll");
        }
    }

    // Dummy read to clear initial state before waiting
    char buf[2];
    for(int i=0; i<NBR_BTN; i++) {
        pread(btn[i], buf, sizeof(buf), 0);
    }

    printf("Waiting for button presses...\n");

    // Event main loop
    while (1) {
        struct epoll_event events[NBR_BTN];

        // Timeout is -1: Block infinitely until an event occurs!
        int n = epoll_wait(epfd, events, 1, -1);

        if (n < 0) {
            perror("epoll_wait error");
            break;
        }

        for (int i = 0; i < n; i++) {
            // read btn file
            pread(btn[i], buf, sizeof(buf), 0);
            if (events[i].data.fd == btn[0]) {
                if (buf[0] == '1') {
                    printf("Decrease led frequency");
                }

            } else if (events[i].data.fd == btn[1]) {
                if (buf[0] == '1') {
                    printf("Reset led frequency");
                }

            } else if (events[i].data.fd == btn[2]) {
                if (buf[0] == '1') {
                    printf("Increase led frequency");
                }
            }
        }
    }

    for (int i=0; i<NBR_BTN; i++) {
        close(btn[i]);
    }
    close(epfd);
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

    struct timespec t1;
    clock_gettime(CLOCK_MONOTONIC, &t1);

    // Setup button thread
    pthread_t btn_thread_inst;
    pthread_create(&btn_thread_inst, NULL, btn_thread, NULL);


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

    }

    pthread_join(btn_thread_inst, NULL);


    return 0;
}
