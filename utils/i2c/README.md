# i2c util工具
## 功能
+ i2c_util scan                            --> scan all /dev/i2c-*
+ i2c_util scan 1                          --> scan all /dev/i2c-1
+ i2c_util dump 1 0x50                     --> dump /dev/i2c-1 dev addr 0x50 reg addr 0x00 0xf0
+ i2c_util dump 1 0x50 0x10 0x90           --> dump /dev/i2c-1 dev addr 0x50 reg addr 0x10 0x90
+ i2c_util read 1 0x50 0x10                --> read /dev/i2c-1 dev addr 0x50 reg addr 0x10
+ i2c_util read 1 0x50 0x10 /16            --> read /dev/i2c-1 dev addr 0x50 reg addr 0x10,16-bit return value
+ i2c_util read 1 0x50 16/0x1000 /16       --> read /dev/i2c-1 dev addr 0x50 reg addr 0x1000,16-bit return value
+ i2c_util write 2 0x50 0 4f               --> writes 0x47 to register 0
+ i2c_util write 2 0x50 0 16/1e4f          --> writes 0x1e47 to register 0
+ i2c_util write 2 0x50 16/3006 16/1e4f    --> writes 0x1e47 to register 0x3006

## 参考
+ [i2c tool链接](https://github.com/mozilla-b2g/i2c-tools/tree/master/tools)
+ [i2cio 链接](https://github.com/michael-heyns/i2cio)
