#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/ioport.h>
#include <linux/io.h>

#define CHIP_ID_BASE_ADDR 0x01c14000
#define TEMPERATURE_SENSOR_BASE_ADDR 0x01C25000
#define ETHERNET_CONTROLLER_BASE_ADDR 0x01C30000

static struct resource* resources[3] = {[0] = 0,};

void ioMemoryMapped_init(void) {
    pr_info("Initialize memory-mapped I/O\n");

    // Declare variables
    unsigned char* registers[3] = {[0] = 0,};
    uint32_t chipid[4] = {[0] = 0,};
    uint32_t temperature = 0;
    uint32_t mac_address[2] = {[0] = 0,};

    // Request memory
    resources[0] = request_mem_region(CHIP_ID_BASE_ADDR, 0x1000, "nanopi - chip ID");
    if (resources[0] == 0) {
        pr_info("Failed to reserve memory region for chip ID\n");
    }
    
    resources[1] = request_mem_region(TEMPERATURE_SENSOR_BASE_ADDR, 0x1000, "nanopi - temperature sensor");
    if (resources[1] == 0) {
        pr_info("Failed to reserve memory region for temperature sensor\n");
    }
    
    resources[2] = request_mem_region(ETHERNET_CONTROLLER_BASE_ADDR, 0x1000, "nanopi - Ethernet controller");
    if (resources[2] == 0) {
        pr_info("Failed to reserve memory region for Ethernet controller\n");
    }


    // Map memory
    registers[0] = ioremap(CHIP_ID_BASE_ADDR, 0x1000);
    if (registers[0] == 0) {
        pr_err("Failed to map processor registers for chip ID\n");
        return;
    }
    registers[1] = ioremap(TEMPERATURE_SENSOR_BASE_ADDR, 0x1000);
    if (registers[1] == 0) {
        pr_err("Failed to map processor registers for temperature sensor\n");
        return;
    }
    
    registers[2] = ioremap(ETHERNET_CONTROLLER_BASE_ADDR, 0x1000);
    if (registers[2] == 0) {
        pr_err("Failed to map processor registers for Ethernet controller\n");
        return;
    }


    // Read values - Chip ID
    chipid[0] = ioread32(registers[0] + 0x200);
    chipid[1] = ioread32(registers[0] + 0x204);
    chipid[2] = ioread32(registers[0] + 0x208);
    chipid[3] = ioread32(registers[0] + 0x20c);
    pr_info(
        "chipid=%08x'%08x'%08x'%08x\n",
        chipid[0], chipid[1], chipid[2], chipid[3]
    );

    // Read values - Temperature
    temperature = -1991 * (int32_t) ioread32(registers[1] + 0x80) / 10 + 223000;
    pr_info(
        "temperature=%d (register value: %d)\n",
        temperature, ioread32(registers[1] + 0x80)
    );

    // Read values - MAC address
    mac_address[0] = ioread32(registers[2] + 0x50);
    mac_address[1] = ioread32(registers[2] + 0x54);
    pr_info(
        "mac-addr=%02x:%02x:%02x:%02x:%02x:%02x\n",
        (mac_address[1] >> 0) & 0xff,
        (mac_address[1] >> 8) & 0xff,
        (mac_address[1] >> 16) & 0xff,
        (mac_address[1] >> 24) & 0xff,
        (mac_address[0] >> 0) & 0xff,
        (mac_address[0] >> 8) & 0xff
    );

    // Unmap memory
    iounmap(registers[0]);
    iounmap(registers[1]);
    iounmap(registers[2]);

    pr_info("Memory-mapped I/O initialized\n");

}

void ioMemoryMapped_exit(void) {
    pr_info("Free memory-mapped I/O\n");

    // Release memory
    if (resources[0] != 0) release_mem_region(CHIP_ID_BASE_ADDR, 0x1000);
    if (resources[1] != 0) release_mem_region(TEMPERATURE_SENSOR_BASE_ADDR, 0x1000);
    if (resources[2] != 0) release_mem_region(ETHERNET_CONTROLLER_BASE_ADDR, 0x1000);
}