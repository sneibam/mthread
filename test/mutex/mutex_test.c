#include <stdio.h>
#include "../mthread.h"

int i=0;

mthread_mutex_t mutex;


void* run (void* arg)
{
	mthread_mutex_lock(&mutex);
	i++;
	mthread_mutex_unlock(&mutex);
	return NULL;
}

int main(int argc, char** argv)
{
	int j;
	mthread_t pids[10];

	mthread_mutex_init (&mutex,NULL);

	for (j = 0; j < 10; j++)
	{
		mthread_create(&(pids[j]), NULL, run, NULL);
	}

	for (j = 0; j < 10; j++)
	{
		mthread_join(pids[j], NULL);
	}

	printf("%d\n", i);
}
