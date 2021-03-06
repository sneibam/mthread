#include <stdio.h>
#include "../../mthread.h"


void *run(void *arg)
{
	printf("SUCCESS\n");
	return NULL;
}

int main(int argc, char** argv)
{
	// Testing the mthread_sem_init function
	mthread_t thread;
	mthread_create(&thread, NULL, run, NULL);
	mthread_sem_t sem;
	printf("Initializing Semaphore...\n");
	mthread_sem_init(&sem, 5);
	printf("Semaphore initialized successfully\n");
	mthread_join(thread, NULL);
	return 0;
}
