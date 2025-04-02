#include <stdio.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/mymodule", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Device opened successfully... \nwaiting ...\n");
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    int interrupt_count = 0;


    fd_set tempfds = readfds;
    int ret = select(fd + 1, &tempfds, NULL, NULL, NULL);
    printf("Called select...\n");
    if (ret < 0) {
        perror("select failed");
        close(fd);
        return 1;
    }

    if (FD_ISSET(fd, &tempfds)) {
        int count;
        read(fd, &count, sizeof(count));
        interrupt_count += count;
        printf("Total interrupts: %d\n", interrupt_count);
    }


    close(fd);
    return 0;
}