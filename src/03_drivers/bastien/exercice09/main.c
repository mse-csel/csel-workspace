#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "skeleton.h"

int main(int argc, char* argv[]) {
    if (argc <= 1) return 0;
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    // Reset the device
    if (ioctl(fd, SKELETON_IO_RESET) < 0) {
        perror("Failed to reset device");
    }

    // Convert a char to an int from the argv
    int value = 0;
    if (argc > 1) {
        value = atoi(argv[2]);
    } else {
        printf("No value provided, using default: %d\n", value);
    }
    // Write a value
    if (ioctl(fd, SKELETON_IO_WR_VAL, value) < 0) {
        perror("Failed to write value");
    }

    // Read the value
    int read_value;
    if (ioctl(fd, SKELETON_IO_RD_VAL, &read_value) < 0) {
        perror("Failed to read value");
    } else {
        printf("Read value: %d\n", read_value);
    }

    close(fd);
    return 0;
}