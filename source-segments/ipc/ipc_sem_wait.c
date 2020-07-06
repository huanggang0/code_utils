#include <stdlib.h>
#include "ipc.h"

static sem_t g_sem_wait = {
	.key = 1234,
	.flag = 0,
	.init_value = -1,
	.fd = -1,
};

int main(void) {
	int ret = -1;
	ret = ipc_sem_init(&g_sem_wait);
	if(ret < 0) {
		printf("ipc sem wait ipc_sem_init failed\n");
		return -1;
	}
	printf("ipc sem wait process start\n");
	while(1) {
		/* wait block */
		ret = ipc_sem_wait(&g_sem_wait,-1);
		/* wait with timeout 3s */
		//ret = ipc_sem_wait(&g_sem_wait,2000);
		/* wait unblock */
		//ret = ipc_sem_wait(&g_sem_wait,0);
		if(ret == 0) {
			printf("ipc sem get semphore\n");
		}else {
			printf("ipc sem wait timeout\n");
		}
		sleep(1);
	}
	return 0;
}
