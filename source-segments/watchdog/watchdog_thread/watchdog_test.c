#include "watchdog.h"

int main(void) {
	unsigned int cnt = 0;
	pthread_t watch_dog;

	pthread_create(&watch_dog,NULL,wdt_check_func,NULL);

	WDT_THREAD_ADD(10,WDT_AC_REBOOT);
	while(1){
		cnt++;
		if(cnt < 10) {
			WDT_THREAD_BEAT();
			sleep(1);
		}
		sleep(1);
		printf("main thread running cnt %d\n",cnt);
	}
	return 0;
}
