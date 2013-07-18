/*************************************************************************
	> File Name: i2c_test.c
	> Author: izobs
	> Mail: ivincentlin@gmail.com
	> Created Time: 2013年07月15日 星期一 15时04分23秒
 ************************************************************************/

#include <stdio.h>
#include <linux/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#define I2C_RETRIES 0x0701
#define I2C_TIMEOUT 0x0702
#define I2C_RDWR    0x0707

struct i2c_msg
{
    unsigned short addr;
    unsigned short flags;
    #define I2C_M_TEN 0x0010
    #define I2C_M_RD  0x0001
    unsigned short len;
    unsigned char *buf;
};

struct i2c_rdwr_ioctl_data
{
    struct i2c_msg *msgs;
    int nmsgs;
};

int main()
{
    int fd,ret;
    struct i2c_rdwr_ioctl_data e2prom_data;
    fd = open("/dev/i2c-0",O_RDWR);
    if(fd < 0)
    {
        perror("open error\n");
    }
    e2prom_data.nmsgs = 2;  /*时序多为2个开始信号*/
    e2prom_data.msgs = (struct i2c_msg*)malloc(e2prom_data.nmsgs*sizeof(struct i2c_msg));
    if(!e2prom_data.msgs)
    {
	perror("malloc error");
	exit(1);
    }

    ioctl(fd,I2C_TIMEOUT,1); /*超时时间*/
    ioctl(fd,I2C_RETRIES,2); /*重复次数*/

    e2prom_data.nmsgs = 1;
    (e2prom_data.msgs[0]).len = 2;
    (e2prom_data.msgs[0]).addr=0x50;//e2prom 设备地址
    (e2prom_data.msgs[0]).flags=0; //write
    (e2prom_data.msgs[0]).buf=(unsigned char*)malloc(2);
    (e2prom_data.msgs[0]).buf[0]=0x10;// e2prom 写入目标的地址
    (e2prom_data.msgs[0]).buf[1]=0x58;//the data to write
    ret=ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);

    if(ret < 0)
    {
	perror("ioctl error");
    }
    sleep(1);

    e2prom_data.nmsgs = 2;
    (e2prom_data.msgs[0]).len=1; //e2prom 目标数据的地址
    (e2prom_data.msgs[0]).addr=0x50; // e2prom 设备地址
    (e2prom_data.msgs[0]).flags=0;//write
    (e2prom_data.msgs[0]).buf[0]=0x10;//e2prom 数据地址
    (e2prom_data.msgs[1]).len=1;//读出的数据
    (e2prom_data.msgs[1]).addr=0x50;// e2prom 设备地址
    (e2prom_data.msgs[1]).flags=I2C_M_RD;//read
    (e2prom_data.msgs[1]).buf=(unsigned char*)malloc(1);//存放返回值的地址。
    (e2prom_data.msgs[1]).buf[0]=0;//初始化读缓冲
    ret=ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);
    if(ret<0)
    {
	perror("ioctl error2");
    }
    printf("buff[0] = %x\n",e2prom_data.msgs[1].buf[0]);

    close(fd);
    return 0;

}
