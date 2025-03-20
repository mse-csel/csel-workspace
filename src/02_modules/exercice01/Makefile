# Part executed when called from kernel build system
ifneq ($(KERNELRELEASE),)
obj-m += mymodule.o          ## name of the generated module
mymodule-objs := skeleton.o  ## list of objets needed by that module
CFLAGS_skeleton := -DDEBUG   ## to enable printing of debugging messages

# Part executed when called from standard make in the module source directory
else
CPU := arm64
KDIR := /buildroot/output/build/linux-5.15.148/
TOOLS := /buildroot/output/host/usr/bin/aarch64-linux-
PWD := $(shell pwd)
# CC := /buildroot/output/host/usr/bin/aarch64-linux-gnu-gcc.br_real


all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(CPU) CROSS_COMPILE=$(TOOLS) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(CPU) CROSS_COMPILE=$(TOOLS)  clean

endif