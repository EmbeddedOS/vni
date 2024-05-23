obj-m += vni.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc receiver.c -o receiver.o
	gcc transmitter.c -o transmitter.o
