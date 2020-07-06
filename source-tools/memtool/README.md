# 读写设备内存地址
## 目的
+ 应用层去配置寄存器
+ 应用层读取寄存器
## 使用方法
+ memtool r phy_addr     
+ 读取物理地址的32位值
+ 
+ memtool r phy_addr len 
+ 读取物理地址连续len个32位值
+ 
+ memtool w phy_addr val 
+ 将val值写入物理地址phy\_addr处	
+ 
+ memtool w phy_addr val len 
+ 将val值写入物理地址phy\_addr开始的len个地址