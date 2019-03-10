#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../mthread.h"
#define NB_THREADS 5

mthread_mutex_t mutex;
mthread_cond_t cond;

void *run_master(void* arg)
{
	sleep(5); // Sleep pour synchroniser, il faut que tout les threads esclaves soit en attente du broadcast
	mthread_mutex_lock(&mutex);
	printf("Master thread is BROADCASTING...\n");
	mthread_cond_broadcast(&cond);
	mthread_mutex_unlock(&mutex);
	return NULL;
}

void *run_slave(void* arg)
{
	long rank = (long) arg;
	mthread_mutex_lock(&mutex);
	printf("Thread %ld is waiting ...\n", rank);
	mthread_cond_wait(&cond, &mutex);
	printf("Thread %ld received signal\n", rank);
	mthread_mutex_unlock(&mutex);
	return NULL;
}

int main(int argc, char** argv)
{

	int j;
	mthread_t* pids = (mthread_t*) malloc(NB_THREADS*sizeof(mthread_t));
	mthread_mutex_init(&mutex, NULL);
	mthread_cond_init(&cond, NULL);

	for (j=1; j < NB_THREADS; j++)
	{
		mthread_create(&(pids[j]), NULL, run_slave,(void*)(size_t) j);
	}	
	j = 0;
	mthread_create(&(pids[0]), NULL, run_master, (void*)(size_t)j);

	for (j=0; j < NB_THREADS; j++)
	{
		mthread_join(pids[j], NULL);
	}
	printf("SUCCESS\n");
	return 0;
}
