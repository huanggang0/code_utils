ARCH = arm

ifeq ($(ARCH), arm)
CC = aarch64-linux-gcc
else
CC = gcc
endif

INC_PATH = -I ./
LIB_PATH = ./lib
OBJ_PATH = obj

CFLAGS = $(INC_PATH)
CFLAGS += -rdynamic -g -Wall -fno-strict-aliasing -Wcast-align -O3 

TARGET = ipc_fifo_read ipc_fifo_write ipc_msg_read ipc_msg_write ipc_sem_wait ipc_sem_post ipc_shm_read ipc_shm_write

LDFLAGS = -lpthread -lm

INCS = $(wildcard $(INC_PATH)/*.h)
SRCS = $(shell find . -name "*.c")

OBJS = $(SRCS:%.c=%.o)

all: $(TARGET)

ipc_fifo_read  : ipc_fifo.c
	$(CC) ipc_fifo_read.c ipc_fifo.c -g -o ipc_fifo_read

ipc_fifo_write : ipc_fifo.o
	$(CC) ipc_fifo_write.c ipc_fifo.c -g -o ipc_fifo_write

ipc_msg_read  : ipc_msg.c
	$(CC) ipc_msg_read.c ipc_msg.c -g -o ipc_msg_read

ipc_msg_write : ipc_msg.c
	$(CC) ipc_msg_write.c ipc_msg.c -g -o ipc_msg_write

ipc_sem_wait  : ipc_sem.c
	$(CC) ipc_sem_wait.c ipc_sem.c -g -o ipc_sem_wait

ipc_sem_post : ipc_sem.c
	$(CC) ipc_sem_post.c ipc_sem.c -g -o ipc_sem_post

ipc_shm_read  : ipc_shm.c
	$(CC) ipc_shm_read.c ipc_shm.c -g -o ipc_shm_read

ipc_shm_write : ipc_shm.c
	$(CC) ipc_shm_write.c ipc_shm.c -g -o ipc_shm_write
	
.PHONY:clean

clean:
	rm -rf *.o $(TARGET)
