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

int main(void)
{

    // Open all button with the right flags
    int btn[NBR_BTN] = {0};
    for(int i=0; i<NBR_BTN; i++) {
        btn[i] = open_btn(GPIO_BTN[i], BTN[i]);
        if (btn[i] < 0) {
            perror("Failed to open button");
            return 1;
        }
    }

    // Create epoll instance to control all button files
    int epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("Failed to create epoll");
        return 1;
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
            return 1;
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
            for (int j = 0; j < NBR_BTN; j++) {
                if (events[i].data.fd == btn[j]) {

                    // Read the new value. pread uses offset 0 so we don't need lseek()
                    pread(btn[j], buf, sizeof(buf), 0);

                    // Print the result. '1' or '0' depends on your hardware pull-up/down resistors
                    if (buf[0] == '1') {
                        printf("Button %d State: HIGH (1)\n", j+1);
                    } else {
                        printf("Button %d State: LOW (0)\n", j+1);
                    }
                }

            }
        }
    }

    for (int i=0; i<NBR_BTN; i++) {
        close(btn[i]);
    }
    close(epfd);
    return 0;
}
