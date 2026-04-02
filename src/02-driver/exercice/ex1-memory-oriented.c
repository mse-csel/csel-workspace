#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <unistd.h>

int ex_memory_oriented(void) {

    int fd = open("/dev/mem", O_RDWR);

    if (fd < 0) {
        printf("Failed to open /dev/mem: %s\n", strerror(errno));
        return 1;
    }

    size_t page_size = getpagesize(); // return the number of byte in a page
    off_t chip_id_addr = 0x01c14200; // physical address
    off_t offset = chip_id_addr % page_size; // get the number of page until the chip is address
    off_t start_page = chip_id_addr - offset; // align with pages

    printf("page_size=0x%x offset=0x%x offset_page=0x%x\n", (unsigned int) page_size, (unsigned int) offset, (unsigned int) start_page);

    // Get register virtual address from /dev/mem of the chip id
    volatile uint32_t* regs = mmap(0, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, start_page);

    if (regs == MAP_FAILED) {
        printf("Failed to mmap: %s\n", strerror(errno));
        return 1;
    }



    uint32_t chipid[4] = {[0] = 0,};

    // Read values - Chip ID
    chipid[0] = *(regs + (0x00 + offset) / sizeof(uint32_t));
    chipid[1] = *(regs + (0x04 + offset) / sizeof(uint32_t));
    chipid[2] = *(regs + (0x08 + offset) / sizeof(uint32_t));
    chipid[3] = *(regs + (0x0c + offset) / sizeof(uint32_t));
    printf(
        "chipid=%08x'%08x'%08x'%08x\n",
        chipid[0], chipid[1], chipid[2], chipid[3]
    );

    // Free space memory of the user¨
    munmap((void*)regs, page_size);

    // Close the file
    close(fd);


    return 0;
}
