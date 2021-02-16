obj-m += hoge.o
all:
	make -C ~/buildroot-/output/build/linux-<ver>/ M=$(PWD) modules
	EXTRA_CFLAGS="-g DDEBUG"
clean:
	make -C <buildroot dir>/output/build/linux-<ver>/ M=$(PWD) clean