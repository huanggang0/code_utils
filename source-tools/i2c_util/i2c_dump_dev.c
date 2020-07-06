#include "i2c_util.h"

bool i2c_dump(uint8_t bus,           /* i2c bus number - (see /dev/i2c-*) */
        uint8_t i2c_addr,      /* i2c device address - range: 0x03 - 0x77 */
        uint16_t start_reg,     /* start reg address */
        uint16_t end_reg       /* end reg address */)      
{
    char fname[15];
    int file_i2c,i = 0,j = 0;
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg msgs[2];
    uint8_t line_res[16],res,buf[sizeof(uint8_t) * 3] = {0}; // max 24bit

    // clear
    memset(&data, 0, sizeof(data));
    memset(&msgs[0], 0, sizeof(struct i2c_msg));
    memset(&msgs[1], 0, sizeof(struct i2c_msg));

    // create filename
    sprintf(fname, "/dev/i2c-%d", bus);

	if(access(fname,R_OK) == -1){
		printf("i2c dev : %s not exist\n",fname);
		return false;
	}

    // open bus
    if ((file_i2c = open(fname, O_RDWR | O_NONBLOCK)) < 0)
    {
        printf("ERROR: Failed to open the i2c bus. Try 'sudo'\n\n");
        return false;
    }

    /* printf title */
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f"
            "    0123456789abcdef\n");

    for(i = start_reg ; i < end_reg ; i+= 16){
        printf("%02x: ", i);
		memset(line_res,0x00,sizeof(line_res));
        for(j = 0; j < 16; j++) {
            if (i+j < start_reg || i+j > end_reg) {
                printf("   ");
                continue;
            }

            // fill in register address
            buf[0] = (uint8_t)((i+j) & 0xFF);

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

            // read
            data.msgs = msgs;
            data.nmsgs = 2;
            if (ioctl(file_i2c, I2C_RDWR, &data) < 0)
            {
                //printf("ERROR: Read failure\n\n");
                //close(file_i2c);
                //return false;
                printf("XX ");
            }else{
				line_res[j] = buf[0];
                printf("%02x ", buf[0]);
            }
        }

        printf("   ");

        for(j = 0; j < 16;j++){
            if (i+j < start_reg || i+j > end_reg) {
                printf(" ");
                continue;
            }
            res = line_res[j];
            if (res < 0)
                printf("X");
            else if ((res & 0xff) == 0x00 || (res & 0xff) == 0xff)
            	printf(".");
            else if ((res & 0xff) < 32 || (res & 0xff) >= 127)
                printf("?");
            else
                printf("%c", res & 0xff);
        }
		printf("\n");
    }

    close(file_i2c);
    return true;
}
