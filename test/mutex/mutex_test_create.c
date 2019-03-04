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
	printf("Testing mthread_mutex create and mthread_join\n");

	mthread_mutex_init (&mutex,NULL);


	mthread_t thread;
	mthread_create(&(thread), NULL, run, NULL);
	mthread_join(thread, NULL);

	return 0;

}
