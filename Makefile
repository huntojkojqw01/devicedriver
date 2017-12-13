obj-m += ofd.o memory.o rsa.o

KDIR = /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules	
	gcc -o main main.c rsa.c
clean:  
	rm -rf *.o *.ko *.mod.* *.symvers *.order main encode decode