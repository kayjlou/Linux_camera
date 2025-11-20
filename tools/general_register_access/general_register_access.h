#ifndef _GENERAL_REGISTER_ACCESS_H_
#define _GENERAL_REGISTER_ACCESS_H_

#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

struct reg_value {
	u8 slave_addr;
	u16 regaddr;
	u8 value;
};

#endif
