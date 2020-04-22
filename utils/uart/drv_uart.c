#include "drv_uart.h"

/*
 * vmin > 0 vtime = 0 read block
 * vmin = 0 vtime > 0 read vtime * 100ms,then return
 * vmin > 0 vtime > 0 read vmin bytes or vtime * 100ms after receiving the fisrt byte
 * vmin = 0 vtime = 0 read unblock
 */

static int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,
    B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300, };
static int name_arr[] = {115200, 38400, 19200, 9600, 4800, 2400, 1200,  300, 
    115200, 38400, 19200, 9600, 4800, 2400, 1200,  300, };

static void uart_set_baudrate (struct termios *opt, uint32_t speed)
{
    int i;
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) { 
        if(speed == name_arr[i]) {         
            cfsetispeed(opt, speed_arr[i]);  
            cfsetospeed(opt, speed_arr[i]);
            break;
        }
    }
    return;
}

static void uart_set_databit(struct termios *opt, uint32_t databit)
{
    opt->c_cflag &= ~CSIZE;
    switch (databit) {
        case 8:
            opt->c_cflag |= CS8;
            break;
        case 7:
            opt->c_cflag |= CS7;
            break;
        case 6:
            opt->c_cflag |= CS6;
            break;
        case 5:
            opt->c_cflag |= CS5;
            break;
        default:
            opt->c_cflag |= CS8;
            break;
    }
    return;
}

static void uart_set_parity(struct termios *opt,uint32_t nParity){
    switch(nParity) {   
        //no parity check
        case PAR_NONE:     
            opt->c_cflag &= ~PARENB;
            break; 
        //odd check
        case PAR_ODD:     
            opt->c_cflag |= PARENB;
            opt->c_cflag |= PARODD;
            opt->c_cflag |= (INPCK | ISTRIP);
            break;
        //even check
        case PAR_EVEN:     
            opt->c_cflag |= (INPCK | ISTRIP);
            opt->c_cflag |= PARENB;
            opt->c_cflag &= ~PARODD;
            break;
        //no parity check
        default:            
            opt->c_cflag &= ~PARENB;
            break;        
    }
    return;
}

static void uart_set_stopbit(struct termios *ops,uint32_t nStop){
    switch(nStop){
        case 1:
            ops->c_cflag &= ~CSTOPB;
            break;
        case 2:
            ops->c_cflag |= CSTOPB;
            break;
        default:
            ops->c_cflag &= ~CSTOPB;
            break;
    }
    return;
}

static void uart_set_raw(struct termios *ops){
    ops->c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    ops->c_oflag &= ~OPOST;
    ops->c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
}

static void uart_set_read_mode(struct termios *ops,uint32_t vtime,unsigned vmin){
    ops->c_cc[VTIME] = vtime;
    ops->c_cc[VMIN] = vmin;
}

#if 0
static int uart_set_block(int fd,int block){
    int flags;
    if((flags = fcntl(fd, F_GETFL, 0)) < 0)
    {
        perror("fcntl");
        return -1;
    }
    if(block == 0)
        flags |= O_NONBLOCK;
    else if(block == 1)
        flags &= (~O_NONBLOCK);
    else{
        perror("invaild arg");
        return -1;
    }

    if(fcntl(fd, F_SETFL, flags) < 0)
    {
        perror("fcntl");
        return -1;
    }
    return 0;
}
#endif

int uart_init(uart_t *uart){
    int ret;
	unsigned int flags = O_RDWR | O_NOCTTY | O_NDELAY;
    struct termios  opt;

    assert(uart != NULL && uart->name != NULL);

    ret = open(uart->name,flags);
    if(ret <= 0){
        printf("uart : %s failed\n",__func__);
        return -1;
    }

    uart->fd = ret;

    tcgetattr(uart->fd, &opt);
    opt.c_cflag |= CLOCAL;
    opt.c_cflag |= CREAD;

    //set usart baudrate
    uart_set_baudrate(&opt,uart->baudrate);

    //set usart parity
    uart_set_parity(&opt,uart->parity);

    //set databit
    uart_set_databit(&opt,uart->databit);

    //set stopbit
    uart_set_stopbit(&opt,uart->stopbit);

    //set raw param
	if(uart->raw == 1){
		uart_set_raw(&opt);
		/* vtime = 0 vmin = 0 */
		uart_set_read_mode(&opt,0,0);
	}

    //enable the new usart option
    tcflush(uart->fd,TCIFLUSH);
    if(tcsetattr(uart->fd,TCSANOW,&opt) < 0){
        perror("tcsetattr failed");
        return -1;
    }
    return 0;
}

static int _uart_read(uart_t *uart,char *buf,int len){
    assert(uart != NULL && uart->fd > 0);

    int ret,total = 0,left = len - total;
    int delay = 0;

    while(total < len){
        ret = read(uart->fd,buf+total,left);
        if(ret > 0){
            total += ret;
            left = len - total;
            delay = 0;
        }else if(ret == 0){
            /* if uart no data within UART_WAIT_MS since last character,then return */
            if (delay == 0){
                usleep(UART_WAIT_MS*1000);
                delay = 1;
                continue;
            }else if(delay == 1){
                break;
            }
        }
        else if(ret < 0){
            break;
        }
    }
    return (total == 0) ? -1 : total;;
}

int uart_read(uart_t *uart,char *buf,int len,int timeout){
    assert(uart != NULL && uart->fd > 0);

    fd_set fs_read;
    int read_len = 0,ret;
    struct timeval tv_timeout;
    if(timeout < 0){
        /* max wait time */
        tv_timeout.tv_sec =  0x7fffffff;
        tv_timeout.tv_usec = 0;
    }else{
        tv_timeout.tv_sec = timeout / 1000;
        tv_timeout.tv_usec = (timeout % 1000) * 1000;
    }

    FD_ZERO(&fs_read);
    FD_SET(uart->fd, &fs_read);

    ret = select(uart->fd + 1, &fs_read, NULL, NULL, &tv_timeout);
    if(ret > 0)  {
        read_len = _uart_read(uart, buf, len);
    }else if(read_len < 0){
        read_len = -1;
    }

    return read_len;
}

int uart_write(uart_t *uart,char *buf,int len){
    assert(uart != NULL && uart->fd > 0);

    return write(uart->fd,buf,len);
}

void uart_close(uart_t *uart){
	assert(uart != NULL && uart->fd > 0);

    close(uart->fd);
}

