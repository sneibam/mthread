#include <stdio.h>
#include "../../mthread.h"
#include <unistd.h>

/* Ping/Pong Test */

/*pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_ping = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_pong = PTHREAD_COND_INITIALIZER;*/

mthread_mutex_t lock;
mthread_cond_t cond_ping;
mthread_cond_t cond_pong;

void *ping(void* arg)
{
	sleep(5);
	while(1)
	{
		mthread_mutex_lock(&lock);
		printf("PING\n");
		mthread_cond_signal(&cond_pong);
		mthread_cond_wait(&cond_ping, &lock);
		mthread_mutex_unlock(&lock);
	}
	return NULL;
}


void *pong(void* arg)
{


	while(1)
	{
		mthread_mutex_lock(&lock);
		//printf("Thread B commence a attendre le signal du thread A\n");
		mthread_cond_wait(&cond_pong, &lock);
		printf("PONG\n");
		mthread_cond_signal(&cond_ping);
		mthread_mutex_unlock(&lock);
	}
	return NULL;
}

int main(int argc, char** argv)
{

	mthread_mutex_init(&lock, NULL);
	mthread_cond_init(&cond_ping, NULL);
	mthread_cond_init(&cond_pong, NULL);
	mthread_t A, B;
	mthread_create(&(A), NULL, ping, NULL);
	mthread_create(&(B), NULL, pong, NULL);
	mthread_join(A, NULL);
	mthread_join(B, NULL);


	return 0;

}
