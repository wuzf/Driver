#
# Makefile for kernel helloworld drivers
#
# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
obj-m += helloworld.o
# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
	KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
	ANDROID_KERNEL_DIR ?= /home/wzf/amlogic_m1_0427/kernel/
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(ANDROID_KERNEL_DIR) M=$(PWD) modules
modules:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
endif
