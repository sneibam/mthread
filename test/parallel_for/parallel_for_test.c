#include "../../mthread.h"
#include <errno.h>
#include <stdio.h>


void* boucle(void* arg)
{
	if (arg == NULL) {
		printf("Erreur !!\n");
		exit(EINVAL);
	}
	int* tab = (int*)arg;
	int i;
	for(i = tab[0]; i <= tab[1]; i+=tab[2]) {
		printf("[i=%d]Coucou\n", i);
	}

	return NULL;
}

int main(int argc, char** argv)
{

	printf("Debut du test\n");
	mthread_parallel_t data;
	data.nb_thread = 4;
	data.schedule = 1; // Static
	mthread_parallel_for(&data, boucle, 0, 9, 1);
	printf("SUCCESS\n");
	return 0;
}
