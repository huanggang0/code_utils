#ifndef __TIME_DEBUG_H__
#define __TIME_DEBUG_H__

#include <sys/time.h>
#include <stdint.h>

#define TIME_RECORD_POINT(n)        \
    struct timeval __time_##n;      \
	gettimeofday(&__time_##n,NULL);

#define TIME_RECORD_STAT(n) ({ 		\
	struct timeval tv1,tv2;			\
	tv1 = __time_##n;				\
	gettimeofday(&tv2,NULL);   		\
	uint32_t all = 0;       		\
	uint32_t sec = 0;      			\
	uint32_t us = 0;				\
	sec = tv2.tv_sec - tv1.tv_sec;	\
	us = tv2.tv_usec - tv1.tv_usec; \
	all = sec*1000000 + us;			\
	all;							\
	});


#endif /* __TIME_DEBUG_H__ */

