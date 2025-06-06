#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

#define COMM_FILE_PATH "/tmp/comm_file"

int read_int_from_keyboard(){
    int value = -1;
    printf("Enter an integer: ");
    while ((scanf("%d", &value) != 1) || (value < 0) || (value > 4)) {
        printf("Invalid input. Please enter an integer between 0 and 4: ");
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

int main(){
    int deux = read_int_from_keyboard();
    while(1){

    }

}
