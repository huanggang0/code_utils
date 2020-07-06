#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/watchdog.h> 
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <pthread.h>

#define WDT_AC_NONE		0
#define WDT_AC_REBOOT	1
#define WDT_AC_PRINT	2

#define WDT_DEV			"/dev/watchdog0"
#define WDT_PERIOD		20
#define WDT_MAX_ITEM	30

typedef struct wdt_conf {
	char *name;
	int timeout;
	unsigned char action;
	unsigned int  last_heart;
	unsigned int  cur_heart;
	unsigned char used;
}wdt_conf_t;

typedef struct wdt{
	char *name;
	int fd;
	int timeout;
	int cnt;
	wdt_conf_t confs[WDT_MAX_ITEM];
}wdt_t;


int wdt_thread_add(char *file,int timeout,int action);
void wdt_thread_beat(char *file);
void *wdt_check_func(void *arg);

#define WDT_THREAD_ADD(timeout,action) do {  \
	wdt_thread_add(__FILE__,timeout,action); \
}while(0)

#define WDT_THREAD_BEAT(timeout,action) do {  \
	wdt_thread_beat(__FILE__); \
}while(0)

#endif /* __WATCHDOG_H__ */
