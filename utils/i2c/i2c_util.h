#ifndef __I2C_UTIL_H__
#define __I2C_UTIL_H__

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

#define NAME        "i2cio"
#define VERSION     "v1.0 " __DATE__

#define HELP_SCAN_BUS	1
#define HELP_DUMP_DEV	2
#define HELP_READ_REG	3
#define HELP_WRITE_REG	4
#define HELP_ALL		5


bool i2c_scan(uint8_t bus);          /* i2c bus number - (see /dev/i2c-*) */

bool i2c_read(uint8_t bus,           /* i2c bus number - (see /dev/i2c-*) */
             uint8_t i2c_addr,      /* i2c device address - range: 0x03 - 0x77 */
             uint8_t reg_width,     /* 1, 2 or 3-byte wide register address (i.e. 8, 16 or 24 bits) */
             uint8_t val_width,     /* 1, 2 or 3-byte wide value to read */
             uint32_t reg_addr,     /* register to read */
             uint32_t* result);     /* where result will be written to */

bool i2c_write(uint8_t bus,          /* i2c bus number - (see /dev/i2c-*) */
              uint8_t i2c_addr,     /* i2c device address - range: 0x03 - 0x77 */
              uint8_t reg_width,    /* 1, 2 or 3-byte wide register address (i.e. 8, 16 or 24 bits) */
              uint8_t val_width,    /* 1, 2 or 3-byte wide value to write */
              uint32_t reg_addr,    /* register to write to */
              uint32_t value);      /* the value to write into the register */

bool i2c_dump(uint8_t bus,           /* i2c bus number - (see /dev/i2c-*) */
        uint8_t i2c_addr,      /* i2c device address - range: 0x03 - 0x77 */
        uint16_t start_reg,     /* start reg address */
        uint16_t end_reg );      /* end reg address */

#endif /* __I2C_UTIL_H__ */
