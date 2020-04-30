#include "drv_i2c.h"

int i2c_init(i2c_t *i2c) {
    assert(i2c->bus >= 0);
    assert(i2c->addr > 0);
    assert(i2c->reg_width > 0);
    assert(i2c->val_width > 0);

    int ret = -1;
    char i2c_dev[64];

    sprintf(i2c_dev,"/dev/i2c-%d",i2c->bus);
    if(access(i2c_dev,F_OK) == -1) {
        printf("util i2c %s not exist\n",i2c_dev);
        return -1;
    }

    if ((ret = open(i2c_dev, O_RDWR | O_NONBLOCK)) < 0) {
        printf("util i2c open %s fail\n",i2c_dev);
        return -1;
    }

    i2c->fd = ret;
    return 0;
}

int i2c_write_reg(i2c_t *i2c,int reg_addr,int value) {
    assert(i2c->fd > 0);
    assert(reg_addr >= 0);
    assert(value >= 0);

    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg msgs;
    int off = 0;
    uint8_t buf[sizeof(uint8_t) * 6] = {0}; // max 3 x 24bit

    // fill in the register address
    if (3 == i2c->reg_width) {
        buf[0] = (uint8_t)(reg_addr >> 16);
        buf[1] = (uint8_t)((reg_addr >> 8) & 0xFF);
        buf[2] = (uint8_t)(reg_addr & 0xFF);
        off = 3;
    }else if (2 == i2c->reg_width) {
        buf[0] = (uint8_t)(reg_addr >> 8);
        buf[1] = (uint8_t)(reg_addr & 0xFF);
        off = 2;
    }else if(1 == i2c->reg_width){
        buf[0] = (uint8_t)(reg_addr & 0xFF);
        off = 1;
    }else {
        printf("util i2c invalid reg width\n");
        return -1;
    }

    // fill in the value
    if (3 == i2c->val_width) {
        buf[off+0] = (uint8_t)(value >> 16);
        buf[off+1] = (uint8_t)((value >> 8) & 0xFF);
        buf[off+2] = (uint8_t)(value & 0xFF);
    } else if (2 == i2c->val_width) {
        buf[off+0] = (uint8_t)(value >> 8);
        buf[off+1] = (uint8_t)(value & 0xFF);
    } else if (1 == i2c->val_width) {
        buf[off+0] = (uint8_t)(value & 0xFF);
    } else {
        printf("util i2c invalid val width\n");
        return -1;
    }

    //register + value detail
    msgs.addr = i2c->addr;
    msgs.flags = 0;
    msgs.len = i2c->reg_width + i2c->val_width;
    msgs.buf = buf;

    data.msgs = &msgs;
    data.nmsgs = 1;
    if (ioctl(i2c->fd, I2C_RDWR, &data) < 0) {
        printf("util i2c write addr:0x%02x reg:0x%02x failed\n",i2c->addr,reg_addr);
        return -1;
    }
    return 0;
}

int i2c_read_reg(i2c_t *i2c,int reg_addr){
    assert(i2c->fd > 0);
    assert(reg_addr >= 0);

    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg msgs[2];
    uint8_t buf[sizeof(uint8_t) * 3] = {0}; // max 24bit
    int result;

    //clear
    memset(&data, 0, sizeof(data));
    memset(&msgs[0], 0, sizeof(struct i2c_msg));
    memset(&msgs[1], 0, sizeof(struct i2c_msg));

    //fill in register address
    if (3 == i2c->reg_width) {
        buf[0] = (uint8_t)(reg_addr >> 16);
        buf[1] = (uint8_t)((reg_addr >> 8) & 0xFF);
        buf[2] = (uint8_t)(reg_addr & 0xFF);
    } else if (2 == i2c->reg_width) {
        buf[0] = (uint8_t)(reg_addr >> 8);
        buf[1] = (uint8_t)(reg_addr & 0xFF);
    } else if (1 == i2c->reg_width){
        buf[0] = (uint8_t)(reg_addr & 0xFF);
    } else {
        printf("util i2c invalid reg width\n");
        return -1;
    }

    // register parameters
    msgs[0].addr = i2c->addr;
    msgs[0].flags = 0;
    msgs[0].len = i2c->reg_width;
    msgs[0].buf = buf;

    // expected value parameters
    msgs[1].addr = i2c->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = i2c->val_width;
    msgs[1].buf = buf;

    // read
    data.msgs = msgs;
    data.nmsgs = 2;
    if (ioctl(i2c->fd, I2C_RDWR, &data) < 0) {
        printf("util i2c read addr:0x%02x reg:0x%02x failed\n",i2c->addr,reg_addr);
        return -1;
    }
    // construct the result
    if (3 == i2c->val_width)
        result = (buf[0] << 16) | (buf[1]  << 8) | buf[2];
    else if (2 == i2c->val_width)
        result = (buf[0] << 8) | buf[1];
    else if (1 == i2c->val_width)
        result = buf[0];
    else{
        printf("util i2c invalid val width\n");
        return -1;
    }
    return result;
}

int i2c_write_regs(i2c_t *i2c,i2c_reg_t *reg,int len) {
    assert(i2c->fd > 0);
    assert(reg != NULL);
    assert(len > 0);

    int ret = -1,i;
    for(i = 0 ; i < len ; i++) {
        ret = i2c_write_reg(i2c,reg[i].addr,reg[i].val);
        if(ret < 0) {
            printf("util i2c write addr:0x%02x reg:0x%02x failed\n",i2c->addr,reg[i].addr);
            return -1;
        }
    }
    return 0;
}

int i2c_read_regs(i2c_t *i2c,i2c_reg_t *reg,int len) {
    assert(i2c->fd > 0);
    assert(reg != NULL);
    assert(len > 0);

    int ret = -1,i;
    for(i = 0 ; i < len ; i++) {
        ret = i2c_read_reg(i2c,reg[i].addr);
        if(ret < 0) {
            printf("util i2c read addr:0x%02x reg:0x%02x failed\n",i2c->addr,reg[i].addr);
            return -1;
        }
        reg[i].val = ret;
    }
    return 0;
}

int i2c_deinit(i2c_t *i2c) {
    assert(i2c->fd > 0);
    i2c->bus = -1;
    i2c->addr = -1;
    i2c->reg_width = -1;
    i2c->val_width = -1;
    return close(i2c->fd);
}

void i2c_print_regs(i2c_t *i2c,i2c_reg_t *reg,int len) {
	assert(i2c->reg_width > 0);
    assert(i2c->val_width > 0);

	int i = 0;
	for(i= 0 ; i < len ; i++) {
		switch(i2c->reg_width) {	
			case 1 :
				printf("addr:0x%02x	",reg[i].addr);
				break;
			case 2:
				printf("addr:0x%04x	",reg[i].addr);
				break;
			case 3:
				printf("addr:0x%08x	",reg[i].addr);
				break;
		}

		switch(i2c->val_width) {	
			case 1 :
				printf("reg:0x%02x",reg[i].val);
				break;
			case 2:
				printf("reg:0x%04x",reg[i].val);
				break;
			case 3:
				printf("reg:0x%08x",reg[i].val);
				break;
		}
		printf("\n");
	}
}
