#include "ipc.h"

int ipc_fifo_init(fifo_t *fifo){
	int ret;
	assert(fifo->path != NULL);

	if(access(fifo->path,F_OK) != 0){
		mkfifo(fifo->path,0777);
	}
	
	ret = open(fifo->path,fifo->mode);
	if(ret < 0) {
		printf("ipc fifo open %s failed\n",fifo->path);
		return -1;
	}

	fifo->fd = ret;

	return 0;
}

int ipc_fifo_read(fifo_t *fifo,char *buf,int size,int timeout){
	assert(fifo->fd > 0);
	
	struct timeval tv;
	fd_set fds;
	int ret = -1;

	if(timeout >= 0) {
		tv.tv_sec =  timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
	} else if (timeout < 0) {
		tv.tv_sec =  0x7fffffff;
		tv.tv_usec = 0;
	}

	FD_ZERO(&fds);
	FD_SET(fifo->fd,&fds);
	ret = select(fifo->fd + 1,&fds,NULL,NULL,&tv);
	if(ret > 0) {
		ret = read(fifo->fd,buf,size);
	}

	return ret;
}

int ipc_fifo_write(fifo_t *fifo,char *buf,int size){
	assert(fifo->fd > 0);

	int ret = -1;
	ret = write(fifo->fd,buf,size);

	return ret;
}

int ipc_fifo_deinit(fifo_t *fifo){
	assert(fifo->fd > 0);

	close(fifo->fd);
	return 0;
}
