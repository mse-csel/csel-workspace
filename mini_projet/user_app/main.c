#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define COMM_FILE_PATH "/tmp/comm_file"
typedef enum {
    STATE_EXIT = 0,
    STATE_CHANGE_MODE = 1,
    STATE_INCREASE_FREQUENCY = 2,
    STATE_DECREASE_FREQUENCY = 3
} State;

int read_int_from_keyboard(){
    int value = -1;
    printf("Enter an integer: ");
    while ((scanf("%d", &value) != 1) || (value < 0) || (value > 3)) {
        printf("Invalid input. Please enter an integer between 0 and 3: ");
        while(getchar() != '\n'); // clear the input buffer
    }
    return value;
}

int write_file(char* buffer){
    int fd = open(COMM_FILE_PATH, O_WRONLY);
    if (fd <0 ){
        perror("Failed to open device for writing");
        return -1;
    }
    ssize_t bytes_write = write(fd, buffer, strlen(buffer));
    if (bytes_write < 0) {
        perror("Failed to write to device");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

void print_menu(){
    pritnf("-------CSEL - mini projet-------\n");
    printf("----------user menu-------------\n");
    printf("Enter a number to choose an option");
    printf("0: EXIT\n");
    printf("1: Change mode\n");
    printf("2: Increase frequency\n");
    printf("3: Decrease frequency\n");
}

int main(){
    State user_input = read_int_from_keyboard();
    
    while(1){
        switch (user_input){

        }

    }

}
