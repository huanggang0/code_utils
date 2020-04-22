#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <assert.h>

#define UART_WAIT_MS	2
#define PAR_NONE		0
#define PAR_ODD			1
#define PAR_EVEN		2

typedef struct uart{
	char *name;
	int fd;
	int	 baudrate;
	char databit;
	char stopbit;
	char parity;
	char raw;
}uart_t;

int uart_init(uart_t *uart);
int uart_read(uart_t *uart,char *buf,int len,int timeout);
int uart_write(uart_t *uart,char *buf,int len);
void uart_close(uart_t *uart); 

#endif /* __DRV_UART_H__ */
