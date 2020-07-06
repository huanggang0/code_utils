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

TARGET = i2c_drv_test

LDFLAGS = -lpthread -lm

INCS = $(wildcard $(INC_PATH)/*.h)
SRCS = $(shell find . -name "*.c")

OBJS = $(SRCS:%.c=%.o)

all: $(TARGET)
	
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS)  $^ -o $@ -L$(LIB_PATH) $(LDFLAGS)
	$(shell if [ ! -d $(OBJ_PATH) ]; then mkdir $(OBJ_PATH); fi;)
	mv $(OBJS) $(OBJ_PATH)
	@echo
	@echo -e "\033[33mBuild $(TARGET) Successfully \033[0m"
	@echo

.PHONY:clean

clean:
	rm -rf $(OBJS) $(TARGET)
