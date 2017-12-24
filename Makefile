obj-m += encoder.o decoder.o

KDIR = /lib/modules/$(shell uname -r)/build

all:
	gcc -o lib/config lib/config.c lib/rsa.c
	./lib/config
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	gcc -o test lib/main.c lib/rsa.c	
module:
	insmod encoder.ko
	insmod decoder.ko
rmmodule:
	rmmod encoder
	rmmod decoder
device:
	mknod /dev/encoder c 50 0
	mknod /dev/decoder c 60 0
	chmod 666 /dev/encoder
	chmod 666 /dev/decoder
rmdevice:
	rm /dev/encoder
	rm /dev/decoder
rebuild:
	./lib/config
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:  
	rm -rf *.o *.ko *.mod.* *.symvers *.order  .*.*.cmd main test lib/config .tmp_versions