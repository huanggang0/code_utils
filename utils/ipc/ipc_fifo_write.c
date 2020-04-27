#include <stdlib.h>
#include "ipc.h"

static fifo_t g_fifo_write = {
	.path = "/tmp/fifo1",
	.mode = O_WRONLY,
	.fd = -1,
};

int main(void) {
	int ret = -1,cnt = 0;
	char buf[128];
	ret = ipc_fifo_init(&g_fifo_write);
	if(ret < 0) {
		printf("ipc fifo write ipc_fifo_init failed\n");
		return -1;
	}
	printf(" ipc fifo write process start\n");
	while(1) {
		/* content is changing */
		sprintf(buf,"ipc fifo write %d",cnt);

		ret = ipc_fifo_write(&g_fifo_write,buf,strlen(buf));
		if(ret > 0) {
			printf("ipc fifo write %d bytes\n",ret);
		}
		/* may cause write timeout */
		sleep(rand() % 3 + 2);
		cnt++;
	}
	return 0;
}
