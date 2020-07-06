#ifndef __BUFFER_QUEUE_H__
#define __BUFFER_QUEUE_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

#define MAX_QUEUE	30
#define MAX_ITEM	30

typedef struct {
	char *name;
	int queue_id;
	int item_num;
	int pos_read;
	int pos_write;
	pthread_mutex_t mutex;
	sem_t sem_read;
	sem_t sem_write;
	char *buf[MAX_ITEM];
	int used;
}buf_queue_t;

int buf_queue_init(char *name,int n);

int buf_queue(char *name,char *buffer);

int buf_dequeue(char *name,char **buffer);

int buffer_queue_deinit(char *name);

#endif /* __BUFFER_QUEUE_H__ */
