/*************************************************************************
	> File Name: at24c02.c
	> Author: izobs
	> Mail: ivincentlin@gmail.com
	> Created Time: 2013年07月16日 星期二 19时37分14秒
 ************************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/delay.h>

#define AT24C02_MAJOR 250

#define DEBUG 1

static int at24c02_major = AT24C02_MAJOR;

/*声明at24c02结构体*/
struct at24c02_dev
{
    struct i2c_client *client;
    unsigned int addr;
    char  name[30];
    unsigned short current_pointer;
    struct cdev cdev;
};
struct at24c02_dev *at24c02_devp;




static int at24c02_open(struct inode *inode,struct file*filp)
{
    filp->private_data = at24c02_devp;
    return 0;
}

static ssize_t at24c02_read(struct file *filp, char __user *buf, size_t size,loff_t *ppos)
{
    return 0;
}



static ssize_t at24c02_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    return 0;
}


static int at24c02_ioctl(struct inode*inodep,struct file *filp,unsigned int cmd,unsigned long arg)
{
    return 0;
}

static int at24c02_release(struct inode*inodep,struct file *filp)
{
    filp->private_data = NULL;
    return 0;
}

static const struct file_operations at24c02_fops =
{
    .owner = THIS_MODULE,
    .open  = at24c02_open,
    .read  = at24c02_read,
    .write = at24c02_write,
    .ioctl = at24c02_ioctl,
    .release = at24c02_release,
};


static void at24c02_setup_cdev(struct at24c02_dev*dev, int index)
{
    int err, devnum = MKDEV(at24c02_major, index);
    #if DEBUG
    printk(KERN_NOTICE"the devnum is %d\n",devnum);
    #endif
    cdev_init(&dev->cdev, &at24c02_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops   = &at24c02_fops;

    err = cdev_add(&dev->cdev, devnum, 1);
    if (err)
        printk(KERN_NOTICE"Error %d adding at24c02 %d", err, index);

}


/*在__devinit probe函数中初始化cdev*/

static int __devinit at24c02_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
    int ret;
#if DEBUG
    printk(KERN_NOTICE"at24c02 probe is start init\n");
#endif

    dev_t devnum = MKDEV(at24c02_major,0);

    if (at24c02_major)
    {
        ret = register_chrdev_region(devnum,1,"at24c02");
    }
    else
    {
        ret = alloc_chrdev_region(&devnum,0,1,"at24c02");
    }

    if(ret < 0)
        return ret;
    at24c02_devp = kmalloc(sizeof(struct at24c02_dev),GFP_KERNEL);
    if(!at24c02_devp)
    {
        ret = -ENOMEM;
        goto fail_malloc;
    }
    memset(at24c02_devp,0,sizeof(struct at24c02_dev));

    at24c02_devp->client = client;

    at24c02_setup_cdev(at24c02_devp,0);
    return 0;

fail_malloc:
    unregister_chrdev_region(devnum,1);
    #if DEBUG
    printk(KERN_NOTICE"fail_malloc\n");
    #endif 
    return ret;

}

/*__devexit _remove函数*/

static int __devexit at24c02_remove(struct i2c_client *client)
{
    cdev_del(&at24c02_devp->cdev);
    kfree(at24c02_devp);
    unregister_chrdev_region(MKDEV(at24c02_major,0),1);
    return 0;
}


static const struct i2c_device_id at24c02_id[] = {
    {"at24c02",0},
    {}
};

MODULE_DEVICE_TABLE(i2c,at24c02_id);

static struct i2c_driver at24c02_driver =
{
    .driver = {
        .name = "at24c02",
        .owner = THIS_MODULE,
    },
    .probe = at24c02_probe,
    .remove = __devexit_p(at24c02_remove),
    .id_table = at24c02_id,
};



static int __init at24c02_init(void)
{
    int ret;
#if DEBUG
    printk(KERN_NOTICE"at24c02 probe is start init\n");
#endif
    ret = i2c_add_driver(&at24c02_driver); 
    return ret;
}

void at24c02_exit(void)
{
    i2c_del_driver(&at24c02_driver);
#if DEBUG
    printk(KERN_NOTICE"at24c02 is rmmod\n");
#endif

}

MODULE_AUTHOR("izobs");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("at24c02 eeprom driver");

module_param(at24c02_major, int, S_IRUGO);

module_init(at24c02_init);
module_exit(at24c02_exit);
