#include "ipc.h"

int ipc_shm_init(shm_t *shm){
	int ret;
	char *addr;
	assert(shm->key > 0);

	ret = shmget(shm->key, 0, 0666);
    if (ret < 0)
    {
        ret = shmget(shm->key, 4096, IPC_CREAT | IPC_EXCL | 0666);
        if (ret < 0) {
            printf("ipc shm %s line %d failed\n",__FILE__,__LINE__);
            return -1;
        }
    }
	shm->fd = ret;
	addr = shmat(shm->fd, 0, 0);
    if (addr == (void *)-1) {
        printf("ipc shm %s line %d failed\n",__FILE__,__LINE__);
        return -1;
    }
	printf("shm id = %d\n",shm->fd);
	shm->addr = addr;
	return 0;
}

int ipc_shm_read(shm_t *shm,char *buf,int len){
	assert(shm->fd > 0);

	memcpy(buf,shm->addr,len);
	return len;
}

int ipc_shm_write(shm_t *shm,char *buf,int len){
	assert(shm->fd > 0);

	memcpy(shm->addr,buf,len);
	return len;
}

int ipc_shm_deinit(shm_t *shm){
	int ret;
	assert(shm->fd > 0);

	ret = shmctl(shm->fd, IPC_RMID, NULL);
    if (ret < 0){
        printf("ipc shm %s line %d failed\n",__FILE__,__LINE__);
    }
	shm->addr = NULL;
	return ret;
}




