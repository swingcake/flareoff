CONFIG_MODULE_SIG=n
CONFIG_MODULE_SIG_ALL=n
KBUILD_CFLAGS += -w

obj-m += main.o
all:
	make -w -C  /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
