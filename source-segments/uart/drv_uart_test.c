#include "drv_uart.h"

static uart_t g_uart = {
	.name = "/dev/ttyPS1",
	.fd = -1,
	.databit = 8,
	.stopbit = 1,
	.parity = PAR_EVEN,
	.baudrate = 115200,
	.raw = 1,
};

static void print_hex(char *buffer,int len){
	int i = 0;
	for(i=0;i<len;i++){
		printf("0x%02x ",buffer[i]);
		if((i+1)%16 == 0)
			printf("\n");
	}
	printf("\n");
	printf("\n");
}

int main(void){
	int ret;
	char buf[256],buf1[256];
	ret = uart_init(&g_uart);
	if(ret < 0){
		printf("uart init failed\r\n");
		return -1;
	}
	while(1){
		/* read unblock */
		//ret = uart_read(&g_uart,buf,sizeof(buf),0);

		/* read block */
		ret = uart_read(&g_uart,buf,sizeof(buf),-1);

		/* read timeout 2000ms */
		//ret = uart_read(&g_uart,buf,sizeof(buf),2000);

		/* read direct */
		//ret = read(g_uart.fd,buf,sizeof(buf));
		if(ret == 0){
			printf("uart no data\n");
			sleep(1);
			continue;
		}
		printf("read %d bytes:\n",ret);
		print_hex(buf,ret);
		sprintf(buf1,"uart read %d bytes",ret);
		uart_write(&g_uart,buf1,strlen(buf1));
	}
	return 0;
}
