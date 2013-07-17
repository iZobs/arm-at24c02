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
static int at24c02_attach_adapter(struct i2c_adapter *adapter);
static int at24c02_detach_client(struct i2c_client*client);

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

/*normal_i2c 记录at24c02的地址,在这块板是0x50*/
static unsigned short normal_i2c[] = {
    0x50,I2C_CLIENT_END
};

static unsigned short ignore[2] = {
    I2C_CLIENT_END,I2C_CLIENT_END
};

static unsigned short probe[2] = {
    I2C_CLIENT_END,I2C_CLIENT_END
};

static struct i2c_client_address_data addr_data = {
    .normal_i2c = normal_i2c,
    .probe      = probe,
    .ignore     = ignore,
    .forces     = NULL,
};


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
    .attach_adapter = at24c02_attach_adapter,
    .detach_client = at24c02_detach_client,
    .id_table = at24c02_id,
};



/*************************************************/
static int at24c02_detect(struct i2c_adapter *adapter,int address,int kind)
{
    int ret;
    struct i2c_client *new_client;

    new_client = at24c02_devp->client;
    new_client->addr = address;
    new_client->adapter = adapter;
    new_client->driver = &at24c02_driver;
    new_client->flags = 0;
    
    ret = i2c_attach_client(new_client);

   /* 
    at24c02_devp->client.driver = &at24c02_driver;
    at24c02_devp->client.addr  = address;
    at24c02_devp->client.adapter  = adapter;
    at24c02_devp->client.flags  = 0;
    strlcpy(at24c02_devp->client.name,"at24c02 eeprom",I2C_NAME_SIZE);
    
    ret = i2c_attach_client(&at24c02_devp->client); 
*/
    if(ret)
        printk(KERN_NOTICE"i2c_attach_client error\n");
    return 0;

}

static int at24c02_detach_client(struct i2c_client *client)
{
    return i2c_detach_client(client);
}


/*at24c02_attach_adapter 执行探测物理设备的功能*/
static int at24c02_attach_adapter(struct i2c_adapter *adapter)
{
    /*参数1是i2c_adapter 指针,参数2是要探测的地址,参数3是具体的探测函数*/
    return i2c_probe(adapter,&addr_data,at24c02_detect);
    
}






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


/*

在__devinit probe函数中初始化cdev

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

__devexit _remove函数

static int __devexit at24c02_remove(struct i2c_client *client)
{
    cdev_del(&at24c02_devp->cdev);
    kfree(at24c02_devp);
    unregister_chrdev_region(MKDEV(at24c02_major,0),1);
    return 0;
}
*/

static int __init at24c02_init(void)
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

    at24c02_setup_cdev(at24c02_devp,0);

    i2c_add_driver(&at24c02_devp); /*i2c_add_driver 将会调用i2c_driver结构体中的at24c02_attach_adapter（）函数*/

    return 0;

fail_malloc:
    unregister_chrdev_region(devnum,1);
    #if DEBUG
    printk(KERN_NOTICE"fail_malloc\n");
    #endif 
    return ret;
}

void at24c02_exit(void)
{
    cdev_del(&at24c02_devp->cdev);
    kfree(at24c02_devp);
    unregister_chrdev_region(MKDEV(at24c02_major,0),1);
    i2c_del_driver(&at24c02_driver);
#if DEBUG
    printk(KERN_NOTICE"at24c02 is rmmod\n");
#endif


}

MODULE_AUTHOR("izobs");
MODULE_LICENSE("Dual BSD/GPL");

module_param(at24c02_major, int, S_IRUGO);

module_init(at24c02_init);
module_exit(at24c02_exit);
