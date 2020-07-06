#include <stdlib.h>
#include "ipc.h"

static shm_t g_shm_write = {
	.key = 12,
	.flag = 0,
	.fd = -1,
};

int main(void) {
	int ret = -1,cnt = 0;
	char buf[128];
	ret = ipc_shm_init(&g_shm_write);
	if(ret < 0) {
		printf("ipc shm write ipc_shm_init failed\n");
		return -1;
	}
	printf("ipc shm write process start\n");
	while(1) {
		/* content is changing */
		sprintf(buf,"ipc shm write %d",cnt);

		ret = ipc_shm_write(&g_shm_write,buf,sizeof(buf));
		if(ret > 0) {
			printf("ipc shm write %d bytes\n",ret);
		}
		/* may cause write timeout */
		sleep(rand() % 3 + 2);
		cnt++;
	}
	return 0;
}
