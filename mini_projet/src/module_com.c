#include "module_com.h"


/*outpout is
"tmp, mode, led_freq[Hz]"
"36.84, manual, 2"
*/
void read_device(char* buffer){

    int fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
    }

    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Failed to read from device");
        close(fd);
    }

    //buffer[bytes_read] = '\0'; // Null-terminate the string
    close(fd);
}


int write_device(char c);