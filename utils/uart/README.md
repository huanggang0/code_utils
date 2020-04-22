# 串口功能封装
## 串口结构体

	typedef struct uart {
		char *name; 待打开设备名称
		int fd;     打开设备描述符
		char databit; 数据位 5/6/7/8
		char stopbit; 停止位 1/2
		char parity;  奇偶校验 PAR_NONE/PAR_EVEN/PAR_ODD
		char raw;	  普通模式/原始模式：普通模式阻塞读，read只能收到回车换行才会返回，原始模式下只要接收到字符就可返回
								       普通模式下，控制字符有效；原始模式下控制字符失效
		int	 baudrate; 波特率
	}uart_t;

## 功能
+ 串口初始化
+ uart_int(uart\_t *)
+ 
+ 串口读取
+ uart_read(&g\_uart,buf,sizeof(buf),0);//超时时间为0
+ uart_read(&g\_uart,buf,sizeof(buf),-1);//超时时间为最大，相当于阻塞读
+ uart_read(&g\_uart,buf,sizeof(buf),2000);//超时时间为2s
+ 
+ 串口发送
+ uart_write(uart\_t *.char *,int)
+ 
+ 串口关闭
+ uart_close(uart\_t *)