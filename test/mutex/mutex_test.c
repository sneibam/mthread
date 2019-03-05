/*
* Premier test des mutex
* Le but de ce programme est de lancer NB_THREADS threads. Chacun de ces threads est censé incrementer
* la variable i. À la fin du traitement la valeur de i devrait etre egale au nombre de threads.
* Cet exemple nous permet de constater l'utilité des mutex. En effet, le mutex nous permettent d'assurer l'exclusion
* mutuelle, et donc eviter que 2 threads n'incrementent pas la variable i en même temps.
*/
#include <stdio.h>
#include "../../mthread.h"
#define NB_THREADS 50
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
