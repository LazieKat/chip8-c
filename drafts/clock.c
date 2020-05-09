#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

int g_var = 1000000, o, n, d;


void *test(void* args)
{
	struct timespec ts;
	
	ts.tv_sec = 16.6666 / 1000;
	ts.tv_nsec = 16.6666 * 1000000;


	while(1)
	{
		nanosleep(&ts, &ts);
		g_var--;
	}
}

int main()
{

	printf("%u\n", g_var);


	pthread_t t;
	pthread_create(&t, NULL, test, NULL);

	while (1)
	{
		o = g_var;
		sleep(1);
		n = g_var;

		d = o - n;
		printf("%d\n", d);
	}
	
	return 0;
}