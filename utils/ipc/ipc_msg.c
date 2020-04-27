#include "ipc.h"

int ipc_msg_init(msg_t *msg){
    int ret;
    assert(msg->key > 0);

    ret = msgget(msg->key, msg->flag);
    if(ret < 0) {
		printf("ipc msg ipc_msg_init failed\n");
		return -1;
    }
	printf("ret = %d\n",ret);
    msg->fd = ret;
    return 0;
}

int ipc_msg_read(msg_t *msg,char *buf,int len,int timeout){
	assert(msg->fd > 0);

	long type;
	int ret,cnt = 0;
	/* msg type */
	type = *((long *)buf);

	if(timeout < 0)
		return msgrcv(msg->fd, buf, len, type, 0);

	/* query every 40 ms */
	while((timeout >= 0) && (cnt <= timeout/40)) {
		ret = msgrcv(msg->fd, buf, len, type, IPC_NOWAIT);
		if(ret <= 0){
			usleep(40*1000);
			cnt++;
		}else {
			return ret;
		}
	}
	return -1;
}

int ipc_msg_write(msg_t *msg,char *buf,int len){
	assert(msg->fd > 0);
    int ret;
    
	/* caution : data length except msg type */
    ret = msgsnd(msg->fd, buf, len - sizeof(long), IPC_NOWAIT);
    if(ret < 0) {
        printf("ipc msg ipc_msg_write failed\n");
        return -1;
    }
    return ret;
}

int ipc_msg_deinit(msg_t *msg){
    int ret;
    assert(msg->fd > 0);
    ret = msgctl(msg->fd, IPC_RMID, NULL);
    if (ret < 0) {
        printf("ipc msg ipc_msg_deinit failed\n");
        return -1;
    }
    return ret;
}
