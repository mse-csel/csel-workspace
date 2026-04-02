#import "/doc/metadata.typ": *

#task(
  [
    Display the processor chip ID, CPU temperature and the MAC adress of the Ethernet controller
  ],
  [
    - Chip ID registers: _0x01c1'4200_ to _0x01c1'420c_
    - 32 bits register of the temperature sensor: _0x01c2'5080_
    - two 32 bits registers of the Ethernet controller MAC address: _0x01c3'0050_ and _0x01c3'0054_

    To calculate the temperature value, there is this formul:
    $
      "temperature" = -1991 dot "register value" / 10 + 223000
    $

    The chip ID can be verified in ```/proc/iomem```.
    The register value of the temperature can be verified in the file: ```/sys/class/thermal/thermal_zone0/temp```.
    The MAC address can be verified with ``` ifconfig```.
  ]
)

The resources are savec in a struct:
```c
static struct resource* resources[3] = {[0] = 0,};
```
resources[0] is reserved for the chip ID, resources[1] for the temperature sensor and resources[2] for the Ethernet controller.

We first allocate the resources with `request_mem_region` function. Then we can map the physical address to a virtual address with `ioremap` function. Finally, we can read the value of the registers with `ioread32` function. The request fail because we have an overlap with the EEPROM, but we can ignore this error because we can still read the registers with `ioremap` function.

```c
// Request the resource at (CHIP_ID_BASE_ADDR)
resources[0] = request_mem_region(CHIP_ID_BASE_ADDR, 0x1000, "nanopi - chip ID");

// Map the physical address (CHIP_ID_BASE_ADDR) to a virtual address (registers[0])
registers[0] = ioremap(CHIP_ID_BASE_ADDR, 0x1000);
```
