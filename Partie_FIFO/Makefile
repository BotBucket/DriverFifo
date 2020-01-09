# Si KERNELRELEASE est defini on peut compiler !

ifneq ($(KERNELRELEASE),)
    obj-m := fifot.o
else
   KERNELDIR ?= /lib/modules/$(shell uname -r)/build
   PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	cc -o testing_program client.c

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm testing_program
endif
