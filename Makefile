obj-m += encoder.o decoder.o

KDIR = /lib/modules/$(shell uname -r)/build

all:
	gcc -o lib/config lib/config.c lib/rsa.c
	./lib/config
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	gcc -o test lib/main.c lib/rsa.c	
install:
	insmod encoder.ko>/dev/null
	insmod decoder.ko>/dev/null
remove:
	rmmod encoder>/dev/null
	rmmod decoder>/dev/null
device:
	rm /dev/encoder
	rm /dev/decoder
	mknod /dev/encoder c 50 0
	mknod /dev/decoder c 60 0
	chmod 666 /dev/encoder
	chmod 666 /dev/decoder
clean:  
	rm -rf *.o *.ko *.mod.* *.symvers *.order  .*.*.cmd main test lib/config .tmp_versions