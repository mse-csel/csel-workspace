#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>

#define DATA_LENGTH 70

static const char* data = "I've got chocolate stuck to the roof of my mouth, so I can't speak\0";

static char data_read[DATA_LENGTH] = {};

int ex_character_oriented(void) {
    printf("Exercice 4 - character oriented\n");

    int ret = 0;

    const char* path = "/dev/toto0\0";

    int fd = open(path, O_RDWR);

    if (fd < 0) {
        printf("Failed to open /dev/toto0: %s\n (maybe you need to create it)\n", strerror(errno));
        return 1;
    }

    ret = write(fd, data, DATA_LENGTH);

    fd = open(path, O_RDWR);

    if(ret < 0) {
        printf("Failed to write\n");
        return 1;
    }

    ret = read(fd, data_read, DATA_LENGTH);

    close(fd);

    printf("Read from device: %s\n", path);
    printf("Content: %s\n", data_read);


    return 0;
}
