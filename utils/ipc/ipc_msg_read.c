#include <stdlib.h>
#include "ipc.h"

static msg_t g_msg_read = {
	.key = 12,
	.flag = 0666,
	.fd = -1,
};

typedef struct msgbuf {
	long type;
	char data[128];
}msg_buf_t;

int main(void) {
	int ret = -1;
	msg_buf_t msg_buf;
	ret = ipc_msg_init(&g_msg_read);
	if(ret < 0) {
		printf("ipc msg read ipc_msg_init failed\n");
		return -1;
	}
	printf("ipc msg read process start\n");
	msg_buf.type = 1;
	while(1) {
		/* read block */
		//ret = ipc_msg_read(&g_msg_read,(char *)&msg_buf,sizeof(msg_buf),-1);
		/* read with timeout 3s */
		ret = ipc_msg_read(&g_msg_read,(char *)&msg_buf,sizeof(msg_buf),3000);
		/* read unblock */
		//ret = ipc_msg_read(&g_msg_read,(char *)&msg_buf,sizeof(msg_buf),0);
		if(ret > 0) {
			printf("ipc msg read %d bytes\n",ret);
			printf("content : %s\n",msg_buf.data);
		}else {
			printf("ipc msg read timeout\n");
		}
		sleep(1);
	}
	return 0;
}
