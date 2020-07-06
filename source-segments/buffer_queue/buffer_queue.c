#include "buffer_queue.h"

static buf_queue_t g_buf_queues[MAX_QUEUE];
static pthread_mutex_t g_mutex;
static int g_mutex_inited = 0;

int buf_queue_init(char *name,int n) {
    int i;
    buf_queue_t *queue;

    assert(name != NULL && n > 0);

    if (g_mutex_inited == 0) {
        pthread_mutex_init(&g_mutex,NULL);
        g_mutex_inited = 1;
    }

    pthread_mutex_lock(&g_mutex);

    for(i=0 ; i <MAX_QUEUE ; i++) {
        queue = g_buf_queues+i;
        if(queue->used == 0) {
			pthread_mutex_init(&(queue->mutex),NULL)
            sem_init(&(queue->sem_read),0,0);
            sem_init(&(queue->sem_write),0,n);
			queue->queue_id = i;
            queue->item_num = n;
            queue->pos_read = 0;
            queue->pos_write = 0;
            queue->name = name;
            queue->used = 1;
            break;
        }
    }

    if(i == MAX_QUEUE) {
        fprintf(stderr,"%s queue full\n",__func__);
        i = -1;
    }

    pthread_mutex_unlock(&g_mutex);
    return i;
}

int buf_queue_id(char *name) {
   	assert(name != NULL);
    buf_queue_t *queue;
    int i;
    for(i=0 ; i <MAX_QUEUE ; i++) {
        queue = g_buf_queues+i;
        if(queue->name != NULL && strcmp(queue->name,name) == 0) {
            break;
        }
    }

    if(i == MAX_QUEUE) {
        fprintf(stderr,"%s queue %s not exist\n",__func__,name);
        return -1;
    }
	return i;
}

int buf_queue(int queue_id,char *buffer){
    assert(queue_id >= 0 && buffer != NULL);
    buf_queue_t *queue = g_buf_queues+queue_id;

	if(queue->used == 0) {
		fprintf(stderr,"%s queue %d not used\n",__func__,queue_id);
		return -1;
	}

    sem_wait(&(queue->sem_write));

    queue->buf[queue->pos_write] = buffer;
    queue->pos_write = (queue->pos_write + 1) % queue->item_num;

    sem_post(&(queue->sem_read));
    return 0;
}

int buf_dequeue(int queue_id,char **buffer){
    assert(queue_id >= 0 && buffer != NULL);
    buf_queue_t *queue = g_buf_queues+queue_id;

	if(queue->used == 0) {
		fprintf(stderr,"%s queue %d not used\n",__func__,queue_id);
		return -1;
	}

    sem_wait(&(queue->sem_read));

    *buffer = queue->buf[queue->pos_read];
    queue->pos_read = (queue->pos_read + 1) % queue->item_num;

    sem_post(&(queue->sem_write));
    return 0;
}

int buffer_queue_deinit(int queue_id) {
	assert(queue_id >= 0);
	buf_queue_t *queue = g_buf_queues+queue_id;

	if(queue->used == 0) {
		fprintf(stderr,"%s queue %d not used\n",__func__,queue_id);
		return -1;
	}

    pthread_mutex_lock(&(queue->mutex));

    queue->used = 0;
    queue->name = NULL;
    sem_destroy(&(queue->sem_read));
    sem_destroy(&(queue->sem_write));

    pthread_mutex_unlock(&(queue->mutex));
	pthread_mutex_destory(&(queue->mutex));
    return 0;
}
