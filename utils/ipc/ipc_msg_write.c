#include <stdlib.h>
#include "ipc.h"

static msg_t g_msg_write = {
	.key = 12,
	.flag = 0,
	.fd = -1,
};

typedef struct msgbuf {
	long type;
	char data[128];
}msg_buf_t;

int main(void) {
	int ret = -1,cnt = 0;
	msg_buf_t msg_buf;
	ret = ipc_msg_init(&g_msg_write);
	if(ret < 0) {
		printf("ipc msg write ipc_msg_init failed\n");
		return -1;
	}
	printf("ipc msg write process start\n");
	msg_buf.type = 1;
	while(1) {
		/* content is changing */
		sprintf(msg_buf.data,"ipc msg write %d",cnt);

		ret = ipc_msg_write(&g_msg_write,(char *)&msg_buf,sizeof(msg_buf));
		if(ret > 0) {
			printf("ipc msg write %d bytes\n",ret);
		}
		/* may cause write timeout */
		sleep(rand() % 3 + 2);
		cnt++;
	}
	return 0;
}
