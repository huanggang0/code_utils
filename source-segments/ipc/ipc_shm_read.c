#include <stdlib.h>
#include "ipc.h"

static shm_t g_shm_read = {
	.key = 12,
	.flag = 0666,
	.fd = -1,
};

int main(void) {
	int ret = -1;
	char buf[128];
	ret = ipc_shm_init(&g_shm_read);
	if(ret < 0) {
		printf("ipc shm read ipc_shm_init failed\n");
		return -1;
	}
	printf("ipc shm read process start\n");
	while(1) {
		ret = ipc_shm_read(&g_shm_read,buf,sizeof(buf));
		if(ret > 0) {
			printf("ipc shm read %d bytes\n",ret);
			printf("content : %s\n",buf);
		}
		sleep(1);
	}
	return 0;
}
