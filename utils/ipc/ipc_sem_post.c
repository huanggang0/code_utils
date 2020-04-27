#include <stdlib.h>
#include "ipc.h"

static sem_t g_sem_post = {
	.key = 1234,
	.flag = IPC_EXCL | IPC_CREAT ,
	.init_value = 100,
	.fd = -1,
};

int main(void) {
	int ret = -1;
	ret = ipc_sem_init(&g_sem_post);
	if(ret < 0) {
		printf("ipc sem post ipc_sem_init failed\n");
		return -1;
	}
	printf("ipc sem post process start\n");
	while(1) {
		ret = ipc_sem_post(&g_sem_post);
		if(ret > 0) {
			printf("ipc sem post semphore\n");
		}
		/* may cause post timeout */
		sleep(rand() % 3 + 2);
	}
	return 0;
}
