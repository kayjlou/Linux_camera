/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "CameraRegisterAccess.h"
#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OV2740_WRITE_ADDRESS (0x6C)
#define OV2740_READ_ADDRESS (0x6D)

CameraRegisterAccess::CameraRegisterAccess() {
    fd = openDevice();

    if (fd < 0) {
        printf("I2C Driver Not loaded!");
    }
}

CameraRegisterAccess::~CameraRegisterAccess() {
}

CameraRegisterAccess& CameraRegisterAccess::getInstance() {
    static CameraRegisterAccess instance;
        return instance;
}

int CameraRegisterAccess::openDevice(void) {
	#ifdef SIMULATE_ALL_HARDWARE
    return 0;
    #else

	int fd = open("/dev/i2c-3",O_RDWR);

	if (-1 == fd) {
		printf("open /dev/i2c-3 error\n");
		return -1;
	}

	return fd;
	#endif
}

int CameraRegisterAccess::I2CMasterRead(int dev_addr, unsigned char *value, unsigned char num) {
	#ifdef SIMULATE_ALL_HARDWARE
    return RET_OK;
    #else

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

	{
		std::lock_guard<std::mutex> lock(cameraAccessMutex);
		ret=ioctl(fd,I2C_RDWR,&i2cdev_data);
	}

	if(ret<0) {
		return RET_FAIL;
	} else {
		ret = RET_OK;
	}

	return ret;
	#endif
}

int CameraRegisterAccess::I2CMasterWrite(int dev_addr, unsigned char *value, unsigned char num) {
	#ifdef SIMULATE_ALL_HARDWARE
    return RET_OK;
    #else

	int ret;
	struct i2c_rdwr_ioctl_data i2cdev_data = {0};
	struct i2c_msg msg;

    msg.addr= dev_addr;
    msg.flags = 0;//write
    msg.len= num;
    msg.buf= value;
    i2cdev_data.nmsgs= 1;
    i2cdev_data.msgs= &msg;
	//printf("write: 0x%04x\n", (value[0]>>8)|value[1]);

	{
		std::lock_guard<std::mutex> lock(cameraAccessMutex);
		ret=ioctl(fd,I2C_RDWR,&i2cdev_data);
	}

	if(ret<0) {
		return RET_FAIL;
	} else {
		ret = RET_OK;
	}

	return ret;
	#endif
}

int CameraRegisterAccess::writeRegister(int SCCBID , int addr, int dat, int b16bitAddress) {
	#ifdef SIMULATE_ALL_HARDWARE
    return RET_OK;
    #else

	int ret = RET_OK;
	unsigned char slaveAddr = (SCCBID>>1)&0xff;

	if (b16bitAddress) {
		unsigned char master_data[3] = {(addr>>8)&0xff, (addr)&0xff, dat&0xff};
		ret = I2CMasterWrite(slaveAddr, master_data, sizeof(master_data));
	}

	return ret;
	#endif
}

/* read 1 byte */
int CameraRegisterAccess::readRegister(int SCCBID, int addr, u8 *dat, int b16bitAddress) {
	#ifdef SIMULATE_ALL_HARDWARE
    return RET_OK;
    #else

	unsigned char slaveAddr = (SCCBID>>1)&0xff;
	unsigned char slave_data[2] = {(addr>>8)&0xff, (addr)&0xff};;
	int ret = RET_OK;

	if (b16bitAddress) {
		ret = I2CMasterRead(slaveAddr, slave_data, 1);
		*dat = slave_data[0]&0xff;
	}

	return ret;
	#endif
}

int CameraRegisterAccess::readRegister(int reg, u8 *value) {
	return readRegister(OV2740_READ_ADDRESS, reg, value, 1);
}

int CameraRegisterAccess::readRegister(u16 reg, u8 *val) {
	return readRegister(OV2740_READ_ADDRESS,(int)reg,val,1);
}

int CameraRegisterAccess::read16BitValue(int regHigh, int regLow, int *value) {
	u8 high, low;
    int ret = readRegister(regHigh, &high);

    if (ret == RET_FAIL) {
        return ret;
    }

    ret = readRegister(regLow, &low);

    if (ret == RET_FAIL) {
        return ret;
    }

    *value = static_cast<int16_t>((high << 8) | low);

    return ret;
}

int CameraRegisterAccess::writeRegister(int reg, int value) {
	int result = writeRegister(OV2740_WRITE_ADDRESS, reg, value, 1);

	return result;
}

int CameraRegisterAccess::write16bitValue(int regHigh, int regLow, int value) {
	int ret = 0;
	ret |= CameraRegisterAccess::getInstance().writeRegister(regHigh, (value >> 8) & 0xff);
    ret |= CameraRegisterAccess::getInstance().writeRegister(regLow, value & 0xff);

	return ret;
}

int CameraRegisterAccess::readRegisterWithSlaveAddr(int slaveAddr, int regAddr, u8 *value) {
	return readRegister(slaveAddr, regAddr, value, 1);
}

int CameraRegisterAccess::writeRegWithSlaveAddr(u8 slaveAddr, u16 regAddr, u8 regValue) {
	return writeRegister((int)slaveAddr, (int)regAddr, (int)regValue, 1);
}

int CameraRegisterAccess::writeRegValueStruct(struct RegValue regvalue) {
	int ret = 0;

	if((0x00 == regvalue.slaveAddr) || (0x01 == regvalue.slaveAddr)){
		Utils::msleep(regvalue.regAddr);
	}else{
		ret = writeRegWithSlaveAddr(regvalue.slaveAddr, regvalue.regAddr, regvalue.value);
	}
	return ret;
}

void CameraRegisterAccess::writeTable(const struct RegValue *regs, unsigned int regnum) {
	for(int i = 0; i < regnum; i++) {
		writeRegValueStruct(regs[i]);
	}
}

int CameraRegisterAccess::writeTableWithAbort(const struct RegValue *regs, unsigned int regnum) {
	int ret;

	for(int i = 0; i < regnum; i++) {
		ret = writeRegValueStruct(regs[i]);

		if (ret == RET_FAIL) {
			break;
		}
	}

	return ret;
}
