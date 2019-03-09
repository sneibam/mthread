#include <stdio.h>
#include "../../mthread.h"


mthread_sem_t sem;

void *run(void *arg)
{
	mthread_sem_wait(&sem);
	printf("SUCCESS\n");
	mthread_sem_post(&sem);
	return NULL;
}

int main(int argc, char** argv)
{
	// Testing the mthread_sem_destroy function
	mthread_t thread;
	mthread_create(&thread, NULL, run, NULL);
	printf("Initializing Semaphore...\n");
	mthread_sem_init(&sem, 5);
	printf("Semaphore initialized successfully\n");
	mthread_join(thread, NULL);
	printf("About to destroy semaphore...\n");
	mthread_sem_destroy(&sem);
	return 0;
}
