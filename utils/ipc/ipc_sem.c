#include "ipc.h"

int ipc_sem_init(sem_t *sem){
	assert(sem->key > 0);

	int ret;
	union semun sem_union;
	ret = semget(sem->key,1, sem->flag | 0666);
	if(ret <= 0){
		ret = semget(sem->key,1,0);
		if(ret <= 0) {
			printf("ipc sem semget failed\n");
			return -1;
		}
	}
	sem->fd = ret;

	/* producer initialize the semphore */
	if(sem->init_value >= 0) {
		sem_union.val = sem->init_value;
		if(semctl(sem->fd, 0, SETVAL, sem_union) == -1){
			printf("ipc sem SETVAL failed\n");
		    return -1;
		}
	}

	return 0;
}

int ipc_sem_post(sem_t *sem){
	assert(sem->fd > 0);

    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1;
    sem_b.sem_flg = SEM_UNDO | IPC_NOWAIT;
    if(semop(sem->fd, &sem_b, 1) == -1){
        printf("ipc sem %s failed\n",__FILE__);
        return -1;
    }
    return 0;
	
}

int ipc_sem_wait(sem_t *sem,int timeout){
	assert(sem->fd > 0);

	int ret,cnt = 0;
	struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = -1;
	if(timeout < 0) {
		sem_b.sem_flg = SEM_UNDO;
		return semop(sem->fd, &sem_b, 1);
	}

	sem_b.sem_flg = SEM_UNDO | IPC_NOWAIT;
	while(timeout > 0 && cnt <= timeout/40) {
		ret = semop(sem->fd, &sem_b, 1);
		if(ret < 0){
			usleep(40*1000);
			cnt++;
			continue;
		}
		return 0;
	}
	return -1;
}

int ipc_sem_deinit(sem_t *sem) {
	assert(sem->fd > 0);
	
    union semun sem_union;
    if(semctl(sem->fd, 0, IPC_RMID, sem_union) == -1){
        printf("ipc sem %s failed\n",__FILE__);
		return -1;
    }
	return 0;
}
