#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FILE_SZ (10 * 1024 * 1024)

static const char* msg2 = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";

int main()
{
    // open again 1st file generated by the main1 application
    int fd     = open("test1.txt", O_RDWR, 0664);
    char* file = mmap(0, FILE_SZ, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    printf("the file after mapping...\n");

    // modify every 10th lines
    for (int i = 0; i < 10000; i += 10)
        memcpy(file + i * strlen(msg2), msg2, strlen(msg2));

    printf("the file after filling...\n");
    close(fd);

    return 0;
}
