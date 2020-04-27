#include "ipc.h"

int ipc_sock_init(sock_t *sock){
    assert(sock->path != NULL);

    int fd = socket(AF_LOCAL, SOCK_STREAM, 0);

    unlink(sock->path);
    struct sockaddr_un serv;
    strcpy(serv.sun_path, sock->path);
    serv.sun_family = AF_LOCAL;
    int ret = bind(fd, (struct sockaddr*)&serv, sizeof(serv));
    if(ret == -1){
        printf("ipc sock %s line %d failed\n",__FILE__,__LINE__);
       	return -1;
    }
    listen(fd, 64);
	return 0;
}

int ipc_sock_read(sock_t *sock,char *buf,int len,int timeout){
}

int ipc_sock_write(sock_t *sock,char *buf,int len){
}

int ipc_sock_deinit(sock_t *sock){
}
