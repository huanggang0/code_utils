#include "watchdog.h"

/* define by user */
static wdt_t g_wdt = {
    .name = WDT_DEV,
    .fd = -1,
    .timeout = WDT_PERIOD,
    .cnt = 0,
};

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

static int wdt_init(wdt_t * wdt){
    int ret = 0;
    assert(wdt->name != NULL && wdt->timeout > 0);

	if(wdt->fd > 0) {
		printf("wdt : open %s already\n",wdt->name);
		return 0;
	}

    ret = open(wdt->name,O_WRONLY);
    if(ret < 0){
        printf("wdt : open %s failed\n",wdt->name);
        return -1;
    }

    wdt->fd = ret;

    /* set watchdog timeout */
    ioctl(wdt->fd, WDIOC_SETOPTIONS, WDIOS_ENABLECARD);
    ioctl(wdt->fd, WDIOC_SETTIMEOUT, &wdt->timeout);

    return 0;
}

static int wdt_feed(void){
    unsigned char food = 1;
    int ret = 0;
    assert(g_wdt.fd > 0);

    ret = write(g_wdt.fd, &food, sizeof(food));
    if(ret != 1) {
        printf("wdt : failed feeding watchdog\n");
        return -1;
    }

    return 0;
}

static int wdt_check(unsigned int second){
    int i = 0,cnt;
    unsigned int cur_heart,last_heart;
    unsigned char action,timeout,used;
	char *name;
    assert(g_wdt.fd > 0);

    cnt = g_wdt.cnt;
    /* query all threads */
    for(i=0 ; i < cnt ; i++){
        cur_heart  = g_wdt.confs[i].cur_heart;
        last_heart = g_wdt.confs[i].last_heart;
        action     = g_wdt.confs[i].action;
        timeout    = g_wdt.confs[i].timeout;
        name       = g_wdt.confs[i].name;
        used       = g_wdt.confs[i].used;

        if((used == 1) && (second % timeout == 0)){
            /* heart not update */
            if(cur_heart == last_heart){
                switch(action){
                    case WDT_AC_NONE :
                        break;

                    case WDT_AC_REBOOT:
                        printf("wdt : thread %s timeout,stop feeding!\n",name);
                        while(1) { sleep(1);};
                        break;

                    case WDT_AC_PRINT:
                        printf("wdt : thread %s timeout!\n",name);
                        break;

                    default:
                        printf("wdt : thread %s timeout,no action!\n",name);
                        break;
                }
            }

            /* update the last heart */
            g_wdt.confs[i].last_heart = cur_heart;
        }

    }
    return 0;
}

static int wdt_deinit(wdt_t * wdt){
    assert(wdt->fd > 0);

    close(wdt->fd);
    memset(wdt,0x00,sizeof(wdt_t));
    return 0;
}

int wdt_thread_add(char *file,int timeout,int action){
    int i = 0;
    unsigned char used = 0;
	char *name;
    assert(file != NULL);

    pthread_mutex_lock(&g_mutex);

    if(g_wdt.cnt == 0)
        memset(g_wdt.confs,0x00,sizeof(wdt_conf_t)*WDT_MAX_ITEM);

    for(i = 0; i < WDT_MAX_ITEM ; i++) {
        used = g_wdt.confs[i].used;
		name = g_wdt.confs[i].name;
		if(used == 1 && strcmp(name,file) == 0) {
			printf("wdt : thread %s add already\n",name);
			break;
		}

        if(used == 0){
            g_wdt.confs[i].timeout = timeout;
            g_wdt.confs[i].action = action;
            g_wdt.confs[i].name = file;
            g_wdt.confs[i].used = 1;
            g_wdt.cnt++;
            break;
        }
    }

    pthread_mutex_unlock(&g_mutex);

    if(i == WDT_MAX_ITEM){
        printf("wdt : no free item to add\n");
        return -1;
    }
    return 0;
}

void wdt_thread_beat(char *file){
    int i = 0;
    unsigned char used = 0;
	char *name;
    assert(file != NULL);

    /* update heart */
    for(i = 0; i < WDT_MAX_ITEM ; i++) {
        used = g_wdt.confs[i].used;
        name = g_wdt.confs[i].name;
        if(used == 1 && strcmp(name,file) == 0){
            g_wdt.confs[i].cur_heart++;
            break;
        }
    }
}

void *wdt_check_func(void *arg){
    unsigned int sec = 0;

    /* init watchdog */
    wdt_init(&g_wdt);
    while(1) {
        sleep(1);
        sec++;
        /* feed dog */
        wdt_feed();
        /* check thread timeout */
        wdt_check(sec);	
    }
}
