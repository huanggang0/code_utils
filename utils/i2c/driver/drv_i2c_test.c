#include "drv_i2c.h"

static i2c_t g_i2c = {
	.bus = 1,
	.addr = 0x20,
	.val_width = 1,
	.reg_width = 1,
	.fd = -1,
};

static i2c_reg_t g_adv7188_regs[20] = {

		{0x0000, 0x00},
		{0x0003, 0x0c},
		{0x0017, 0x41},
		{0x0019, 0xfa},
//		{0x001d, 0x47},
		{0x001d, 0x07},
		{0x003a, 0x17},
		{0x003b, 0x71},
		{0x003d, 0xa2},
		{0x003e, 0x6a},
		{0x003f, 0xa0},
		{0x00f3, 0x01},
		{0x00f9, 0x03},

};

static i2c_reg_t g_regs[20] = {

		{0x0000, 0x00},
		{0x0003, 0x00},
		{0x0017, 0x00},
		{0x0019, 0x00},
//		{0x001d, 0x47},
		{0x001d, 0x00},
		{0x003a, 0x00},
		{0x003b, 0x00},
		{0x003d, 0x00},
		{0x003e, 0x00},
		{0x003f, 0x00},
		{0x00f3, 0x00},
		{0x00f9, 0x00},

};

int main(void) {
	int ret = -1;
	ret = i2c_init(&g_i2c);
	if(ret < 0) {
		printf("i2c test : i2c init failed\n");
		return -1;
	}
	ret = i2c_write_regs(&g_i2c,g_adv7188_regs,12);
	if(ret < 0) {
		printf("i2c test : i2c write failed\n");
		return -1;
	}
	ret = i2c_read_regs(&g_i2c,g_regs,12);
	if(ret < 0) {
		printf("i2c test : i2c write failed\n");
		return -1;
	}
	i2c_print_regs(&g_i2c,g_regs,12);
	return 0;
}
