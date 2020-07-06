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

static void help(int cmd)
{
    printf("\n");
	if(cmd == HELP_ALL) {
		printf("Scans, Reads and Writes I2c busses in 8, 16 and 24-bit data and value modes.\n");
		printf("Syntax:\n");
		printf("    i2c_util scan | dump | read | write <parameters>\n");
		printf("\n");
	}

	if(cmd == HELP_ALL || cmd == HELP_SCAN_BUS) {
		printf("'scan' syntax: i2c_util scan [b]\n");
		printf("    b           = (optional)The bus number taken from the device filename.\n");
		printf("                  Example: The number 5 means/dev/i2c-5\n");
		printf("\n");
	}

	if(cmd == HELP_ALL || cmd == HELP_DUMP_DEV) {
		printf("'dump' syntax:i2c_util dump b addr [start end]\n");
		printf("    b           = The bus number taken from the device filename.\n");
		printf("                  Example: The number 5 means/dev/i2c-5\n");
		printf("    addr        = The I2C device address\n");
		printf("                  Example: 38 (or 0x38)\n");
		printf("    [start end] = An optional argument,device register address range\n");
		printf("                  Example: 0x00 0x20\n");
		printf("\n");
	}

	if(cmd == HELP_ALL || cmd == HELP_READ_REG) {
		printf("'read' syntax:i2c_util read b addr [w/]reg [w/]\n");
		printf("    b           = The bus number taken from the device filename.\n");
		printf("                  Example: The number 5 means/dev/i2c-5\n");
		printf("    addr        = The I2C device address\n");
		printf("                  Example: 38 (or 0x38)\n");
		printf("    [w/]        = An optional width specifier: Options are: 8/, 16/, 24/\n");
		printf("                  Example: 16/3006 (or 16/0x3006) means 16-bit value 0x3006.\n");
		printf("                  If nothing specified, the 8/ is assumed.\n");
		printf("    reg         = The I2C device register to read\n");
		printf("                  Example: 52 (or 0x52) means register 0x52\n\n");
		printf("                  The stand-alone [w/] is the width of the result\n");
		printf("\n");
	}

	if(cmd == HELP_ALL || cmd == HELP_WRITE_REG) {
		printf("'write' syntax:i2c_util write b addr [w/]reg [w/]value\n");
		printf("    b           = The bus number taken from the device filename.\n");
		printf("                  Example: The number 5 means/dev/i2c-5\n");
		printf("    addr        = The I2C device address\n");
		printf("                  Example: 38 (or 0x38)\n");
		printf("    [w/]        = An optional width specifier: Options are: 8/, 16/, 24/\n");
		printf("                  Example: 16/3006 (or 16/0x3006) means 16-bit value 0x3006.\n");
		printf("                  If nothing specified, the 8/ is assumed.\n");
		printf("    reg         = The I2C device register to read\n");
		printf("                  Example: 52 (or 0x52) or 16/3006 or 24/0x3f4408\n");
		printf("    value       = The value to write into the register\n");
		printf("                  Example: 2d (or 0x2d) or 24/3fff5 or 16/ff04\n");
		printf("\n");
	}

	if(cmd == HELP_ALL) {
		printf("Other commands:\n");
		printf("    --version | -h | --help\n\n");

		printf("Examples:\n");
		printf("    i2c_util scan                	--> scans all /dev/i2c-*\n");
		printf("    i2c_util scan 2                 --> scans /dev/i2c-2\n");
		printf("    i2c_util dump 2 70              --> dump /dv/i2c-2 device 0x70,register 0x00 to 0xf0\n");
		printf("    i2c_util dump 2 70 0x10 0x90    --> dump /dv/i2c-2 device 0x70,register 0x10 to 0x90\n");
		printf("    i2c_util read 2 70 0            --> reads /dev/i2c-2 device 0x70, register 0\n");
		printf("    i2c_util read 2 70 0 16/        --> ..as above but request a 16-bit return value\n");
		printf("    i2c_util read 2 70 16/3006 /16  --> ..as above but from 16-bit register 0x3006\n");
		printf("    i2c_util write 2 70 0 4f              --> writes 0x47 to register 0\n");
		printf("    i2c_util write 2 70 0 16/1e4f         --> writes 0x1e47 to register 0\n");
		printf("    i2c_util write 2 70 16/3006 16/1e4f   --> writes 0x1e47 to register 0x3006\n\n");
	}
}

static int hex2dec(char* hexaddr)
{
    int addr;
    sscanf(hexaddr, "%x", &addr);
    return addr;
}

int main(int argc, char** argv)
{
	char devname[32];
	int i = 0;
    if (argc == 1 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        help(HELP_ALL);
        return 0;
    }
    else if (strcmp(argv[1], "--version") == 0)
    {
        puts(VERSION);
        return 0;
    }
	/* i2c bus scan */
    else if (strcmp(argv[1], "scan") == 0)
    {
        if (argc == 3){
			i2c_scan(atoi(argv[2]));
            return 0;
		}else if(argc == 2){
			do{
				sprintf(devname,"/dev/i2c-%d",i);
				if(access(devname,R_OK) == -1){
					break;
				}
				i2c_scan(i);
				i++;
			}while(1);
		}else{
			help(HELP_SCAN_BUS);
		}
        return 0;
    }
	/* i2c dump dev */
	else if (strcmp(argv[1], "dump") == 0)
    {
		printf("\n");
        if (argc == 4) {
			i2c_dump(atoi(argv[2]),hex2dec(argv[3]),0x00,0xf0);
            return 1;
        }else if (argc == 6){
			i2c_dump(atoi(argv[2]),hex2dec(argv[3]),hex2dec(argv[4]),hex2dec(argv[5]));
            return 1;
		}else {
			help(HELP_DUMP_DEV);
			return 0;
		}
    }
	/* i2c read/write reg */
	else if(strcmp(argv[1], "read") == 0 || strcmp(argv[1], "write") == 0){
		if(strcmp(argv[1], "read") == 0){
			if(argc < 5 || argc > 6) {
				help(HELP_READ_REG);
				return 0;
			}
		}else if(strcmp(argv[1], "write") == 0){
			if(argc != 6) {
				help(HELP_WRITE_REG);
				return 0;
			}
		}

        // bus
        uint8_t bus = atoi(argv[2]);

        // i2c device address
        uint8_t i2caddr = hex2dec(argv[3]);

        // register (width and value)
        uint8_t reg_width = 1;
        uint32_t reg_addr;

        int i = 4;
        if (strncmp(argv[i], "24/", 3) == 0)
        {
            reg_addr = (uint32_t)hex2dec(&argv[i][3]);
            reg_width = 3;
        }
        else if (strncmp(argv[i], "16/", 3) == 0)
        {
            reg_addr = (uint32_t)hex2dec(&argv[i][3]);
            reg_width = 2;
        }
        else if (strncmp(argv[i], "8/", 2) == 0)
            reg_addr = (uint32_t)hex2dec(&argv[i][2]);
        else
            reg_addr = (uint32_t)hex2dec(argv[i]);

        // return width
        uint8_t val_width = 1;
        uint32_t value;

        if (strcmp(argv[1], "read") == 0)
        {
            // get optional value width
            if (argc == 6)
            {
                int i = 5;
                if (strcmp(argv[i], "24/") == 0)
                    val_width = 3;
                else if (strcmp(argv[i], "16/") == 0)
                    val_width = 2;
            }

            // read and print
            if (i2c_read(bus, i2caddr, reg_width, val_width, reg_addr, &value))
            {
                if (val_width == 3)
                    printf("0x%06X\n", value & 0xFFFFFF);
                else if (val_width == 2)
                    printf("0x%04X\n", value & 0xFFFF);
                else
                    printf("0x%02X\n", value & 0xFF);
                return 0;
            }
        }
        else if (argc == 6 && strcmp(argv[1], "write") == 0)
        {
            int i = 5;
            if (strncmp(argv[i], "24/", 3) == 0)
            {
                value = (uint32_t)hex2dec(&argv[i][3]);
                val_width = 3;
            }
            else if (strncmp(argv[i], "16/", 3) == 0)
            {
                value = (uint32_t)hex2dec(&argv[i][3]);
                val_width = 2;
            }
            else if (strncmp(argv[i], "8/", 2) == 0)
                value = (uint32_t)hex2dec(&argv[i][2]);
            else
                value = (uint32_t)hex2dec(argv[i]);

            // write
            if (i2c_write(bus, i2caddr, reg_width, val_width, reg_addr, value))
            {
                printf("OK\n");
                return 0;
            }
        }
        else
        {
            printf("ERROR: Invalid command\n\n");
        }
    }else{
		help(HELP_ALL);
	}
    return 1;
}


