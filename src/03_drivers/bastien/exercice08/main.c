#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc <= 1) return 0;
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        printf("Could not open %s: error=%i\n", argv[1], fd);
        return -1;
    }

    size_t psz     = getpagesize();
    off_t dev_addr = 0x01c14200;
    off_t ofs      = dev_addr % psz;
    off_t offset   = dev_addr - ofs;
    printf(
        "psz=%lx, addr=%lx, offset=%lx, ofs=%lx\n", psz, dev_addr, offset, ofs);

    /* map to user space nanopi internal registers */
    volatile uint32_t* regs =
        mmap(0, psz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (regs == MAP_FAILED)  // (void *)-1
    {
        printf("mmap failed");
        return -1;
    }

    uint32_t chipid[4] = {
        regs[0], // Offset 0x00
        regs[1], // Offset 0x04
        regs[2], // Offset 0x08
        regs[3]  // Offset 0x0C
    };

    // Print the Chip-ID in a formatted way
    printf("NanoPi NEO Plus2 Chip-ID: %08x-%08x-%08x-%08x\n",
           chipid[0], chipid[1], chipid[2], chipid[3]);

    munmap((void*)regs, psz);
    close(fd);

    return 0;
}