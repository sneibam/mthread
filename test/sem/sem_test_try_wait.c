#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "../mthread.h"
#define NB_THREADS 10
#define SEM_COUNTER 5

mthread_sem_t sem;

void* run (void* arg)
{
	long rank = (long)arg;
	if (mthread_sem_trywait(&sem) == EINVAL) {
		printf("Coucou, je suis le thread %ld, sem value = %d\n", rank, sem.value);
		//mthread_yield();
		sleep(10);
		mthread_sem_post(&sem);
	} else {
		fprintf(stderr, "TRY LOCK failed for thread %ld, the semaphore value is %d\n",rank, sem.value);
	}
	return NULL;
}

int main(int argc, char** argv)
{
	int i;
	mthread_t pids[NB_THREADS];
	printf("Testing the Implementation of try_wait\n");

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

