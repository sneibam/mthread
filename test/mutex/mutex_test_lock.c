#include <stdio.h>
#include <stdlib.h>
#include "../mthread.h"
#define NB_THREADS 5
int i=0;

mthread_mutex_t mutex;


void* run (void* arg)
{
	long rank = (long) arg;
	printf("Tentative de prise du verrou par le thread %ld.\n", rank);
	mthread_mutex_lock(&mutex);
	printf("Le verrou a été pris par le thread %ld.\n", rank);
	i++;
	printf("Le verrou est pret à etre libéré par le thread %ld.\n", rank);
	mthread_mutex_unlock(&mutex);
	printf("Le verrou a été libéré par le thread %ld.\n", rank);
	return arg;
}

int main(int argc, char** argv)
{
	int j;
	mthread_t* pids = (mthread_t*)malloc(NB_THREADS*sizeof(mthread_t));

	mthread_mutex_init (&mutex,NULL);

	for (j = 0; j < NB_THREADS; j++)
	{
		mthread_create(&(pids[j]), NULL, run, (void*)(size_t)j);
	}

	for (j = 0; j < NB_THREADS; j++)
	{
		mthread_join(pids[j], NULL);
	}
	mthread_mutex_destroy (&mutex);

	printf("%d\n", i);
}
