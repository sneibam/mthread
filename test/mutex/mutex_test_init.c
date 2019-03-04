#include <stdio.h>
#include "../mthread.h"

mthread_mutex_t mutex;

void* run (void* arg)
{
	printf("SUCCESS\n");
	return NULL;
}

int main(int argc, char** argv)
{
	printf("Testing mthread_init function\n");

	mthread_mutex_init (&mutex,NULL);
	printf("The mutex has been initialized\n");

	mthread_t thread;
	mthread_create(&(thread), NULL, run, NULL);
	mthread_join(thread, NULL);

	return 0;

}
