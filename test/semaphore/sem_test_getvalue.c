#include <stdio.h>
#include <unistd.h>
#include "../mthread.h"
#define NB_THREADS 10
#define SEM_COUNTER 5

mthread_sem_t sem;

void* run (void* arg)
{
	long rank = (long)arg;
	mthread_sem_wait(&sem);
	printf("Coucou, je suis le thread %d\n", rank);
	mthread_yield();
	int value_sem;
	mthread_sem_getvalue(&sem, &value_sem);
	if (value_sem == sem.value) printf("CORRECT\n");
	else printf("INCORRECT\n");
	mthread_sem_post(&sem);
	return NULL;
}

int main(int argc, char** argv)
{
	int i;
	mthread_t pids[NB_THREADS];
	printf("Testing the Implementation of the sem_getvalue() \n");

	mthread_sem_init(&sem,SEM_COUNTER);
	printf("The semaphore has been initialized with a counter of %d.\n", SEM_COUNTER);

	for (i = 0; i < NB_THREADS; i++) {
		mthread_create(&(pids[i]), NULL, run, (void*)(size_t)i);
	}
	for (i = 0; i < NB_THREADS; i++) {
		mthread_join(pids[i], NULL);
	}

	mthread_sem_destroy(&sem);

	return 0;

}

