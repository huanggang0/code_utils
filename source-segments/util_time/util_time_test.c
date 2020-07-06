#include <stdio.h>
#include <unistd.h>

#include "util_time.h"

int main(void) {
    int t;
    sleep(2);
    TIME_RECORD_POINT(1);
    sleep(1);
    TIME_RECORD_POINT(2);
    sleep(2);
    TIME_RECORD_POINT(3);
    sleep(1);
    TIME_RECORD_POINT(4);
    sleep(1);
    TIME_RECORD_POINT(5);
    sleep(2);
    t = TIME_RECORD_STAT(1);
    printf("takes %d us\n",t);
    t = TIME_RECORD_STAT(1);
    printf("takes %d us\n",t);
    t = TIME_RECORD_STAT(2);
    printf("takes %d us\n",t);
    t = TIME_RECORD_STAT(3);
    printf("takes %d us\n",t);
    t = TIME_RECORD_STAT(4);
    printf("takes %d us\n",t);
    t = TIME_RECORD_STAT(5);
    printf("takes %d us\n",t);
	return 0;
}
