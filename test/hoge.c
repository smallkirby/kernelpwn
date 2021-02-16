#include<linux/errno.h>
#include<linux/file.h>
#include<linux/fs.h>
#include<linux/miscdevice.h>
#include<linux/module.h>
#include<linux/slab.h>


static int hoge_open(struct inode *inode, struct file *filp)
{
	printk("hoge_open() is called");
	return 0;
}

static int hoge_release(struct inode *inode, struct file *filp)
{
	printk("hoge_release is called");
	return 0;
}

static long hoge_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned long *mora = 0x32764531e1e217f8;
	char *ptrs[0x100];
	int ix;
	switch(cmd){
		case 0:
			printk("hoge_ioctl(): %lx", *mora);
			break;
		case 1:
			printk("hoge_ioctl() kmalloc(0x80)\n");
			for(ix=0; ix!=0x100; ++ix){
				ptrs[ix] = kmalloc(0x80, GFP_KERNEL);
				memset(ptrs[ix], 'A', 0x80);
				if(ix %0x10 == 0){
					printk("hoge: allocated @ %p\n", ptrs[ix]);
				}
			}
			for(ix=0; ix!=0x20; ++ix){
				kfree(ptrs[ix]);
			}
			break;

		default:
		return -ENOTTY;
	};
}

static const struct file_operations hoge_fops = {
	.owner = THIS_MODULE,
	.open = hoge_open,
	.release = hoge_release,
	.unlocked_ioctl = hoge_ioctl,
};

static struct miscdevice hoge_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "hoge",
	.fops = &hoge_fops,
};

static int __init hoge_init(void)
{
	printk("Module hoge is installed.");
	return misc_register(&hoge_device);
}

static void __exit hoge_exit(void)
{
	misc_deregister(&hoge_device);
}

module_init(hoge_init);
module_exit(hoge_exit);
MODULE_AUTHOR("NIRUGIRI");
MODULE_LICENSE("GPL");