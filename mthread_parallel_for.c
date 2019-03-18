#include "mthread_internal.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int
mthread_parallel_for(mthread_parallel_t* data, void *(*routine)(void*), int index_begin, int index_end, int step)
{

	int i;
	if (data == NULL || routine == NULL) return EINVAL;
	// Allocation de la liste de threads
	mthread_t* threads = (mthread_t*)malloc(data->nb_thread*sizeof(mthread_t));

	if (data->schedule == 1) // Static
	{
		int iteration_per_thread = ((index_end-index_begin+1)/step)/data->nb_thread;

		fprintf(stderr, "[PARALLEL_FOR] Nombre d'iteration par thread = %d\n", iteration_per_thread);
		int k=index_begin;
		for (i = 0; i < data->nb_thread; i++) {
			int* args = (int*)malloc(3*sizeof(int));
			args[0] = k;
			args[1] = k+(iteration_per_thread*step) - 1;
			args[2] = step;
			k = k+(iteration_per_thread*step);
			fprintf(stderr, "[PARALLEL_FOR] Appel a la routine par le thread {%d} avec les arguments suivants: %d, %d, %d\n",i, args[0], args[1], args[2]);
			mthread_create(&(threads[i]), NULL, routine, (void*)args);
		}

		for (i = 0; i < data->nb_thread; i++) {
			mthread_join(threads[i], NULL);
		}
	}

	return 0;
}
