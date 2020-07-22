obj-m += memo.o
all:
		make -C /home/wataru/linux-stable/ M=$(PWD) modules
clean:
		make -C /home/wataru/linux-stable/ M=$(PWD) clean
