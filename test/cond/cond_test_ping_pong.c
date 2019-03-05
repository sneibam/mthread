#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/* Ping/Pong Test */

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_ping = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_pong = PTHREAD_COND_INITIALIZER;
volatile int

void *ping(void* arg)
{
	sleep(10);
	while(1)
	{
		pthread_mutex_lock(&lock);
		printf("PING\n");
		pthread_cond_signal(&cond_pong);
		pthread_cond_wait(&cond_ping, &lock);
		pthread_mutex_unlock(&lock);
	}
	return NULL;
}


void *pong(void* arg)
{


	while(1)
	{
		pthread_mutex_lock(&lock);
		//printf("Thread B commence a attendre le signal du thread A\n");
		pthread_cond_wait(&cond_pong, &lock);
		printf("PONG\n");
		pthread_cond_signal(&cond_ping);
		pthread_mutex_unlock(&lock);
	}
	return NULL;
}

int main(int argc, char** argv)
{

	pthread_t A, B;
	pthread_create(&(A), NULL, ping, NULL);
	pthread_create(&(B), NULL, pong, NULL);
	pthread_join(A, NULL);
	pthread_join(B, NULL);


	return 0;

}
