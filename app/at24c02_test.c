/*************************************************************************
	> File Name: at24c02_test.c
	> Author: izobs
	> Mail: ivincentlin@gmail.com 
	> Created Time: 2013年07月17日 星期三 16时30分32秒
 ************************************************************************/

#include <stdio.h>
#include <linux/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc,char *argv[])
{
    int i,fd;
    unsigned char value[512]; /*驱动buffer的最大值为512*/
    value[0] = 0x12;
    value[1] = 0xff;
    value[3] = 0x26;
    value[4] = 0x24;
    value[5] = 0x2a;
    value[6] = 0x22;
    value[7] = 0x20;
    value[8] = 0x29;
    value[9] = 0x24;
    value[10] = 0x53;


    fd = open("/dev/at24c02",O_RDWR);
    if(fd < 0)
    {
        printf("app:open at24c02 device failed\n");
        exit(EXIT_FAILURE);
    }
    write(fd,value,6);
    for(i = 0;i < 6;i++)
    {
        printf("app:write reg[%d] data:%x to at24c02\n",i,value[i]);
        value[i] = 0x00;
    }
    sleep(1);

    i=0;

    read(fd,value,6);
    for(i = 0; i < 6; i++)
    {
        printf("app:read reg[%d] data:%x from at24c02\n",i,value[i]);
    }
    close(fd);
    return 0;
}

