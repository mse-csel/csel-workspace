#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

#include "led_control.h"
#include "switch_control.h"

#include "process.h"

int main(int argc, char** argv){
    // Create daemon
    


    printf("hello there\n");
    setlogmask(LOG_UPTO(LOG_NOTICE));   //discards info and debug
    openlog("app_epoll", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    long period=0;
    if(argc>=2){
        period = atoi(argv[1]) * 1000000;
    }else{
        period = 50000000; // default to 50ms
    }
    printf("period: %ld\n", period);
    epoll_process();
    printf("have a nice day\n");
    return 0;
}