obj-m += memo.o
all:
		make -C /home/wataru/linux-stable/ M=$(PWD)  modules
		EXTRA_CFLAGS="-g DDEBUG"
clean:
		make -C /home/wataru/linux-stable/ M=$(PWD) clean
