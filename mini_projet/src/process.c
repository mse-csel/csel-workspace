#include "process.h"

static int DUTY_CYCLE_ON = 50;
//--- epoll approach starts here

static int MAX_EVENT_FOR_SINGLE_LOOP = 8;
static int EPOLL_WAIT_TIMEOUT = -1; // wait forever until an event occurs
#define READ_BUFFER_SIZE 128

static void add_to_epoll(int epoll_fd, int fd, uint32_t flags){
    long tmp_long;
    struct epoll_event events = {
        .events = flags,
        .data.fd = fd
    };
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &events) < 0){
        perror("epoll_ctl");
        close(epoll_fd);
    }else{
        //consume any prior event
        read(fd, &tmp_long, sizeof(tmp_long));
    }
}

void epoll_process(long period){
    long default_period = period;   //ns
    long current_period = default_period;
    long delta_period = default_period * 10 / 100;  //10% of default period
    char buf;
    int led_status;
    int status_led_fd, power_led_fd, k1_fd, k2_fd, k3_fd;
    int timer_on_fd, timer_off_fd, timer_led_fd;
    int i, epoll_status, tmp_fd;
    long tmp_long;
    struct epoll_event events[MAX_EVENT_FOR_SINGLE_LOOP];
    int epoll_fd = epoll_create1(0);
    // create fd and define associated events

    ssd1306_init();

    //switch K1
    k1_fd = open_switch(K1, GPIO_K1);
    add_to_epoll(epoll_fd, k1_fd, EPOLLPRI);
    //switch K2
    k2_fd = open_switch(K2, GPIO_K2);
    add_to_epoll(epoll_fd, k2_fd, EPOLLPRI);
    //switch K3
    k3_fd = open_switch(K3, GPIO_K3);
    add_to_epoll(epoll_fd, k3_fd, EPOLLPRI);
    //timer to activate the led
    timer_on_fd = start_timer(current_period, 0);
    add_to_epoll(epoll_fd, timer_on_fd, EPOLLIN | EPOLLPRI);
    //timer to deactivate the led
    timer_off_fd = start_timer(current_period, DUTY_CYCLE_ON);
    add_to_epoll(epoll_fd, timer_off_fd, EPOLLIN | EPOLLPRI);

    //timer to blink the led after presses
    timer_led_fd = start_timer(0, 0);
    add_to_epoll(epoll_fd, timer_led_fd, EPOLLIN | EPOLLPRI);

    //led fd
    status_led_fd = open_led(STATUS_LED, GPIO_STATUS_LED);
    power_led_fd = open_led(POWER_LED, GPIO_POWER_LED);

    led_status = 1;
    while(1){
        epoll_status = epoll_wait(epoll_fd, events, MAX_EVENT_FOR_SINGLE_LOOP, EPOLL_WAIT_TIMEOUT);
        if(epoll_status > 0){
            for(i=0;i<epoll_status;i++){
                tmp_fd = events[i].data.fd;
                // execute behaviour for fd
                if((tmp_fd == timer_on_fd) || (tmp_fd == timer_off_fd)){
                    read(tmp_fd, &tmp_long, sizeof(tmp_long));
                    write(status_led_fd, (led_status ? "1" : "0"), 1);
                    led_status = !led_status;
                }else if((tmp_fd == k1_fd) || (tmp_fd == k2_fd) || (tmp_fd == k3_fd)){
                    lseek(tmp_fd, 0, SEEK_SET);
                    read(tmp_fd, &buf, 1);
                    //reset if k2, reduce period if k1, increase period if k3
                    if(tmp_fd == k1_fd){
                        current_period -= delta_period;
                        syslog(LOG_NOTICE, "increasing blinking frequency\n");
                        write(power_led_fd, "1", 1); // notifiy user of button press
                        //update_timer(timer_led_fd, LED_ON_TIME, 0);
                        printf("K1 - increase rotation\n");

                    }else if(tmp_fd == k2_fd){
                        current_period = default_period;
                        syslog(LOG_NOTICE, "reset blinking frequency\n");
                        write(power_led_fd, "1", 1); // notifiy user of button press
                        //update_timer(timer_led_fd, LED_ON_TIME, 0);
                        printf("K2 - decrease rotation\n");
                    }else{
                        current_period += delta_period;
                        syslog(LOG_NOTICE, "lowering blinking frequency\n");
                        write(power_led_fd, "1", 1); // notifiy user of button press
                        //update_timer(timer_led_fd, LED_ON_TIME, 0);
                        printf("K3 - change mode\n");
                    }
                    update_timer(timer_on_fd, current_period, 0);
                    update_timer(timer_off_fd, current_period, DUTY_CYCLE_ON);
                } 
                else if(tmp_fd == timer_led_fd){
                    read(tmp_fd, &tmp_long, sizeof(tmp_long));
                    write(power_led_fd, "0", 1);
                    update_timer(timer_led_fd, 0, 0);
                    //printf("timer led off\n");
                    // Need to be fixed, timer never gets its actual value
                }
                else{
                    break;
                }
            }
            // manage event here

                ssd1306_set_position (0,0);
                ssd1306_puts("mini_projet");
        }else if(0 == epoll_status){
            //timeout
        }else{
            if( EINTR == errno ){
                // ignorable errors
                continue;
            }else{
                // other errors
                break;
            }
        }
    }
    // good practice would require to unregister file descriptors and close them
    return;
}

//--- epoll approache ends here