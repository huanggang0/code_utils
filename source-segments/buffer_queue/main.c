#include "buffer_queue.h"

void buffer_produce_func(void *arg) {
	char str[64];
	int cnt = 0;
	buf_queue_init("test1",5);
	while(1) {
		sprintf(str,"data %d",cnt);
		cnt++;
	}
}

int main(void) {
}
