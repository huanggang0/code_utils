#ifndef __IPC_H__
#define __IPC_H__

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <assert.h>
#include <sys/sem.h>
#include <sys/shm.h>

typedef struct fifo {
	char *path;
	int mode;
	int fd;
}fifo_t;

typedef struct msg {
	key_t key;
	int flag;
	int fd;
}msg_t;

typedef struct shm {
	key_t key;
	int flag;
	int fd;
	char *addr;
}shm_t;

typedef struct sem {
	key_t key;
	int flag;
	int init_value;
	int fd;
}sem_t;

typedef struct sock {
	char *path;
	int server;
	int fd;
	int connected;
}sock_t;


union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
};

int ipc_fifo_init(fifo_t *fifo);
int ipc_fifo_read(fifo_t *fifo,char *buf,int size,int timeout);
int ipc_fifo_write(fifo_t *fifo,char *buf,int size);
int ipc_fifo_deinit(fifo_t *fifo);

int ipc_msg_init(msg_t *msg);
int ipc_msg_read(msg_t *msg,char *buf,int size,int timeout);
int ipc_msg_write(msg_t *msg,char *buf,int size);
int ipc_msg_deinit(msg_t *msg);

int ipc_sem_init(sem_t *sem);
int ipc_sem_post(sem_t *sem);
int ipc_sem_wait(sem_t *sem,int timeout);
int ipc_sem_deinit(sem_t *sem);

int ipc_shm_init(shm_t *shm);
int ipc_shm_read(shm_t *shm,char *buf,int len);
int ipc_shm_write(shm_t *shm,char *buf,int len);
int ipc_shm_deinit(shm_t *shm);


#endif /* __IPC_H__ */
