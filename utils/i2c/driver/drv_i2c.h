#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <assert.h>

typedef struct i2c_reg {
	unsigned int addr;//max 16-bit register address
	unsigned int val;//max 16-bit register data
}i2c_reg_t;

typedef struct i2c {
	int bus;
	int addr;
	int reg_width;
	int val_width;
	int fd;
}i2c_t;

int i2c_init(i2c_t *i2c);
int i2c_write_reg(i2c_t *i2c,int reg_addr,int value);
int i2c_write_regs(i2c_t *i2c,i2c_reg_t *reg,int len);
int i2c_read_reg(i2c_t *i2c,int reg_addr);
int i2c_read_regs(i2c_t *i2c,i2c_reg_t *reg,int len);
int i2c_deinit(i2c_t *i2c);
void i2c_print_regs(i2c_t *i2c,i2c_reg_t *reg,int len);

#endif /* __DRV_I2C_H__ */
