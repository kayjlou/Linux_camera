/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef CAMERA_REGISTER_ACCESS_H
#define CAMERA_REGISTER_ACCESS_H

#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mutex>

#ifndef SIMULATE_ALL_HARDWARE
#include <linux/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#endif

#define RET_FAIL    		(0)
#define RET_OK      		(1)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct RegValue {
    u8 slaveAddr;
    u16 regAddr;
    u8 value;
};

class CameraRegisterAccess {
public:
    CameraRegisterAccess(const CameraRegisterAccess&) = delete;
    CameraRegisterAccess& operator=(const CameraRegisterAccess&) = delete;
    static CameraRegisterAccess& getInstance();

    int readRegister(int reg, u8 *value);
    int readRegister(u16 reg, u8 *val);
    int read16BitValue(int regHigh, int regLow, int *value);
    int readRegisterWithSlaveAddr(int slaveAddr, int regAddr, u8 *value);
    int writeRegister(int reg, int value);
    int write16bitValue(int regHigh, int regLow, int value);
    void writeTable(const struct RegValue *regs, unsigned int regnum);
    int writeTableWithAbort(const struct RegValue *regs, unsigned int regnum);
    int writeRegWithSlaveAddr(u8 slaveAddr, u16 regAddr, u8 regValue);
private:
    int fd;
    std::mutex cameraAccessMutex;

    CameraRegisterAccess();
    ~CameraRegisterAccess();
    static int openDevice(void);
    int I2CMasterRead(int dev_addr, unsigned char *value, unsigned char num);
    int I2CMasterWrite(int dev_addr, unsigned char *value, unsigned char num);
    int writeRegister(int SCCBID , int addr, int dat, int b16bitAddress);
    int readRegister(int SCCBID, int addr, u8 *dat, int b16bitAddress);
    int writeRegValueStruct(struct RegValue regvalue);
};

#endif
