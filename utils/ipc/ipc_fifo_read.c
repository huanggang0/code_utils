#include <stdlib.h>
#include "ipc.h"

static fifo_t g_fifo_read = {
	.path = "/tmp/fifo1",
	.mode = O_RDONLY,
	.fd = -1,
};

int main(void) {
	int ret = -1;
	char buf[128];
	ret = ipc_fifo_init(&g_fifo_read);
	if(ret < 0) {
		printf("ipc fifo read ipc_fifo_init failed\n");
		return -1;
	}
	printf(" ipc fifo read process start\n");
	while(1) {

		ret = ipc_fifo_read(&g_fifo_read,buf,sizeof(buf),3000);
		if(ret > 0) {
			printf("ipc fifo read %d bytes\n",ret);
			printf("content : %s\n",buf);
		}else {
			printf("ipc fifo read timeout\n");
		}
		sleep(1);
	}
	return 0;
}
