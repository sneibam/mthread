#include <stdio.h>
#include "../../mthread.h"

void *run(void* arg)
{
	printf("SUCCESS\n");
	return NULL;
}

int main(int argc, char** argv)
{
	mthread_t thread;
	mthread_create(&thread, NULL, run, NULL);
	mthread_cond_t cond;
	printf("Initliazing the condition ...\n");
	mthread_cond_init(&cond, NULL);
	printf("the condition has been initialized successfully\n");
	mthread_join(thread, NULL);
	return 0;
}
