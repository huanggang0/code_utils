/*

   MIT License

   Copyright (c) 2018 Michael Heyns

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

*/

#include "i2c_util.h"

static bool i2c_read_test(int fd,uint8_t i2c_addr){
	struct i2c_rdwr_ioctl_data data;
	struct i2c_msg msgs[2];
	uint8_t buf[sizeof(uint8_t) * 3] = {0}; // max 24bit
	// register parameters
	msgs[0].addr = i2c_addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = buf;

	// expected value parameters
	msgs[1].addr = i2c_addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = 1;
	msgs[1].buf = buf;

	//reg address 0x00
	buf[0] = (uint8_t)(0x00);

	// read
	data.msgs = msgs;
	data.nmsgs = 2;
	if (ioctl(fd, I2C_RDWR, &data) < 0)
	{
		//printf("ERROR: Read failure\n\n");
		//close(file_i2c);
		return false;
	}
	return true;
}

bool i2c_scan(uint8_t bus)
{
	uint8_t buffer[4];
	char fname[15];
	int file_i2c;

	// create filename
	sprintf(fname, "/dev/i2c-%d", bus);

	printf("\n%s:\n", fname);

	if(access(fname,R_OK) == -1){
		printf("i2c dev : %s not exist\n",fname);
		return false;
	}

	// open device file
	if ((file_i2c = open(fname, O_RDWR | O_NONBLOCK)) < 0)
	{
		printf("ERROR: Failed to open the i2c bus. Try 'sudo'\n\n");
		return false;
	}

	// run through all the possible I2C addresses (0x03-0x77) and show the
	// ones accupied by a device driver and the ones in user space
	printf("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
	for (uint8_t addr = 0; addr < 0x7F; addr += 16)
	{
		printf("%02X: ", addr);
		for (uint8_t offset = 0; offset < 16; offset++)
		{
			if ((addr + offset) >= 0x01 && (addr + offset) <= 0x7e)
			{
				if (ioctl(file_i2c, I2C_SLAVE, addr + offset) < 0)
				{
					printf("DD-");   /* device + driver */
				}
				else
				{
					uint8_t length = 1;
					/* scan way 1 : not alway available */
					if (read(file_i2c, buffer, length) == length){
						printf("%02X ", addr + offset);
						/* scan way 2 : in case of way 1 failure */
					}else if(i2c_read_test(file_i2c,addr + offset) == true){
						printf("%02X ", addr + offset);
					}
					else
						printf("-- ");
				}
			}
			else
				printf("   ");
		}
		printf("\n");
	}
	printf("    --=No device  DD=Device+Driver  03-77=Available\n");

	close(file_i2c);
	printf("\n");

	return true;
}


