#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    if (argc <= 1) return 0;
    int fd;
    int interrupt_count = 0;
    fd_set read_fds;
    struct timeval timeout;

    // Open the device file
    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    printf("Monitoring interrupts from %s...\n", argv[1]);

    while (1) {
        // Initialize the file descriptor set
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds); // Add the file descriptor to the set

        // Set timeout (optional, can be NULL for blocking indefinitely)
        timeout.tv_sec = 5;  // Wait for 5 seconds
        timeout.tv_usec = 0;

        // Use select to wait for an interrupt
        int ret = select(fd + 1, &read_fds, NULL, NULL, &timeout);
        if (ret < 0) {
            perror("select failed");
            close(fd);
            return EXIT_FAILURE;
        } else if (ret == 0) {
            printf("Timeout occurred, no interrupt detected.\n");
        } else {
            // Check if the file descriptor is set
            if (FD_ISSET(fd, &read_fds)) {
                // Read from the device to acknowledge the interrupt
                char buffer[1];
                if (read(fd, buffer, sizeof(buffer)) < 0) {
                    perror("Failed to read from device");
                    close(fd);
                    return EXIT_FAILURE;
                }

                // Increment and display the interrupt count
                interrupt_count++;
                printf("Interrupt detected! Total count: %d\n", interrupt_count);
            }
        }
    }

    // Close the device file
    close(fd);
    return EXIT_SUCCESS;
}