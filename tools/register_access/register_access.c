#include "register_access.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RET_FAIL    		(0)
#define RET_OK      		(1)
#define OV2740_WRITE_ADDRESS (0x6C)
#define OV2740_READ_ADDRESS (0x6D)

static int fd;

static void msleep(unsigned int msecond)
{
	usleep(msecond*1000);
}

int open_device(void)
{
	fd=open("/dev/i2c-3",O_RDWR);
	if(-1 == fd) {
		printf("open /dev/i2c-3 error\n");
		return -1;
	}
	return fd;
}


int I2CMaster_Read(int dev_addr, unsigned char *value, unsigned char num)
{
    int ret = RET_OK;
    int i = 0;
	struct i2c_rdwr_ioctl_data i2cdev_data = {0};
	struct i2c_msg msgs[2] = {0};
	unsigned char reg_addr[2] = {0};

	memcpy(reg_addr, value, 2);

    msgs[0].addr= dev_addr;
    msgs[0].flags = 0;//write
    msgs[0].len= 2;
    msgs[0].buf= reg_addr;
    msgs[1].addr= dev_addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len= num;
    msgs[1].buf= value;
    i2cdev_data.nmsgs= 2;

    i2cdev_data.msgs= msgs;
	ret=ioctl(fd,I2C_RDWR,&i2cdev_data);
	if(ret<0)
	{
		return RET_FAIL;
	}else {
		ret = RET_OK;
	}

	return ret;
}


int I2CMaster_Write(int dev_addr, unsigned char *value, unsigned char num)
{
	int ret;
	struct i2c_rdwr_ioctl_data i2cdev_data = {0};
	struct i2c_msg msg;

    msg.addr= dev_addr;
    msg.flags = 0;//write
    msg.len= num;
    msg.buf= value;
    i2cdev_data.nmsgs= 1;
    i2cdev_data.msgs= &msg;
	printf("write: 0x%04x\n", (value[0]>>8)|value[1]);
	ret=ioctl(fd,I2C_RDWR,&i2cdev_data);
	if(ret<0)
	{
		return RET_FAIL;
	} else {
		ret = RET_OK;
	}
	return ret;
}



int WriteRegister(int SCCBID , int addr, int dat, int b16bitAddress)
{
	int ret = RET_OK;
	unsigned char slaveAddr = (SCCBID>>1)&0xff;
	if (b16bitAddress)
	{
		unsigned char master_data[3] = {(addr>>8)&0xff, (addr)&0xff, dat&0xff};
		ret = I2CMaster_Write(slaveAddr, master_data, sizeof(master_data));
	}

	return ret;
}


/* read 1 byte */
int ReadRegister(int SCCBID, int addr, u8 *dat, int b16bitAddress)
{
	unsigned char slaveAddr = (SCCBID>>1)&0xff;
	unsigned char slave_data[2] = {(addr>>8)&0xff, (addr)&0xff};;
	int ret = RET_OK;
	if (b16bitAddress)
	{
		ret = I2CMaster_Read(slaveAddr, slave_data, 1);
		*dat = slave_data[0]&0xff;
	}

	return ret;
}

static void stopStream(void)
{
	WriteRegister(OV2740_WRITE_ADDRESS, 0x0100, 0x00, 1);
}

static void startStream(void)
{
	WriteRegister(OV2740_WRITE_ADDRESS, 0x0100, 0x01, 1);
}

/*static int ov2740ReadRegister(u16 reg, u8 *value)
{
	return ReadRegister(OV2740_READ_ADDRESS, (int)reg, value, 1);
}

static int ov2740WriteRegister(u16 reg, u8 value)
{
	stopStream();
	msleep(100);
	int result = WriteRegister(OV2740_WRITE_ADDRESS, (int)reg, (int)value, 1);
	msleep(100);
	startStream();

	return result;
}*/

static int ov2740ReadRegister(int reg, u8 *value)
{
	return ReadRegister(OV2740_READ_ADDRESS, reg, value, 1);
}

static int ov2740WriteRegister(int reg, int value)
{
	//stopStream();
	msleep(100);
	int result = WriteRegister(OV2740_WRITE_ADDRESS, reg, value, 1);
	msleep(100);
	//startStream();

	return result;
}

void printMenu() {
	printf("\nCommand Menu:\n");
	printf("1 - Read Register\n");
	printf("2 - Write Register\n");
	printf("3 - Exit\n");
	printf("Select an option:");
}

void commandExecution() {
	int choice;
	//u16 reg;
	u8 value;
	int reg;
	int valuew;

    while (1) {
        printMenu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // Eingabepuffer leeren
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
 
        switch (choice) {
            case 1:
                printf("Enter register address (hex): ");
                //scanf("%hx", &reg);
				scanf("%x", &reg);
				ov2740ReadRegister(reg, &value);
                printf("Register 0x%X: 0x%X\n", reg, value);
                break;
            case 2:
                printf("Enter register address (hex): ");
                //scanf("%hx", &reg);
				scanf("%x", &reg);
                printf("Enter value to write (hex): ");
                //scanf("%hhx", &value);
				scanf("%x", &valuew);
                ov2740WriteRegister(reg, valuew);
                break;
            case 3:
                printf("Exiting...\n");
                return;
            default:
                printf("Invalid option. Please select a valid menu option.\n");
        }
    } 
}

int main()
{
	int ret = RET_OK;
	int datatemp = 0;

	fd = open_device();
    if(fd < 0)
    {
        printf("I2C Driver Not loaded!");
        return 0;
    }

	commandExecution();
	
	return 0;
}
