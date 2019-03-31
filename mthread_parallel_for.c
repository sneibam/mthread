#include "mthread_internal.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int
mthread_parallel_for(mthread_parallel_t* data, void *(*routine)(void*), int index_begin, int index_end, int step)
{

	int i;
	if (data == NULL || routine == NULL) return EINVAL;
	// Allocation de la liste de threads
	mthread_t* threads = (mthread_t*)malloc(data->nb_thread*sizeof(mthread_t));

	if (data->schedule == 1) // Static
	{

		// Nombre d'iterations au total
		int number_of_iterations = (index_end-index_begin+1)/step;

		// Initialisattion du tableau qui contiendra le nombre exact d'iteration par thread
		int* iterations_per_threads = (int*)malloc(data->nb_thread*sizeof(int));
		if (number_of_iterations % data->nb_thread == 0) /* Si le nombre de thread est un diviseur du nombre d'itération alors, on a un équilibre de charge parfait,
			chaque thread executera le meme nombre de d'iterations*/
		{
			for (i=0; i < data->nb_thread; i++) {
				iterations_per_threads[i] = number_of_iterations / data->nb_thread;
			}
		} else { // Sinon
			/* On recupere le reste de la division entre le nombre d'iteration à faire au total et le nombre de thread
				Cette valeur correspond au nombre d'iteration a rajouter a chaque thread. Ex: Supponsons que l'on a 10 itérations et 4 threads. 10%4 = 2;
				Le quotient de cette division étant égale à 2, chaque thread éxécutera alors au minimun 2 itérations. Les itérations restantes seront réparties un à un*/
			int val = number_of_iterations % data->nb_thread;

			for (i=0; i < data->nb_thread; i++) {
				iterations_per_threads[i] = (number_of_iterations - val) / data->nb_thread;
			}
			i = 0;
			while (val > 0)
			{
				iterations_per_threads[i]++;
				val--;
				i++;
				i = (i == data->nb_thread) ? 0 : i;
			}
		}
		for (i = 0; i < data->nb_thread; i++) {
			fprintf(stderr, "[PARALLEL_FOR] Le thread %d va éxécuter %d itérations\n", i, iterations_per_threads[i]);
		}

		// A ce niveau là, chaque thread sait exactement le nombre d'iteration qu'il va executer.
		int k=index_begin; // Le premier index
		for (i = 0; i < data->nb_thread; i++) {
			int* args = (int*)malloc(3*sizeof(int)); // Allocation mémoire de la liste des arguments. Ces arguments sont l'index de la premiere iteration, l'index de la derniere iteration et le pas
			args[0] = k;
			args[1] = k+(iterations_per_threads[i]*step) - 1;
			args[2] = step;
			k = k+(iterations_per_threads[i]*step); // On met à jour la valeur de k pour qu'elle prenne l'index de la premiere iterarion du thread suivant.
			fprintf(stderr, "[PARALLEL_FOR] Appel a la routine par le thread {%d} avec les arguments suivants: %d, %d, %d\n",i, args[0], args[1], args[2]);
			// On creer le thread avec la routine et les arguments associes au thread.
			mthread_create(&(threads[i]), NULL, routine, (void*)args);
		}


	} else if (data->schedule == 2) // Dynamic Scheduling
	{
		// Nombre d'iterations au total
		int number_of_iterations = (index_end-index_begin+1)/step;
		int number_of_threads_remaining = data->nb_thread; // Initialisation de la variablle number_of_threads_remaining
		int k=0;
		//int index_thread_to_wait = 0;
		while(number_of_iterations > 0)
		{
			if (number_of_threads_remaining == 0)
			{
				for (i=0; i < data->nb_thread; i++) {
					mthread_join(threads[i], NULL);
					number_of_threads_remaining++;
				}
			}
			int chunk_size_for_thread = data->chunk_size;
			if (number_of_iterations < chunk_size_for_thread)
				// Si le nombre d'itérations restantes est inféreieure au chunksize alors le thread suivant executera toutes les iterations restantes
			{
				chunk_size_for_thread = number_of_iterations;
			}
			int i = data->nb_thread - number_of_threads_remaining; // Index du thread
			int* args = (int*)malloc(3*sizeof(int)); // Allocation mémoire de la liste des arguments. Ces arguments sont l'index de la premiere iteration, l'index de la derniere iteration et le pas
			args[0] = k;
			args[1] = k+(chunk_size_for_thread*step) - 1;
			args[2] = step;
			k = k+(chunk_size_for_thread*step); // On met a jour la valeur de k pour qu\'elle prenne l\'index de la premiere iteration du thread suivant.
			fprintf(stderr, "[PARALLEL_FOR] Appel a la routine par le thread {%d} avec les arguments suivants: %d, %d, %d\n",i, args[0], args[1], args[2]);
			// On crée le thread avec la routine et les arguments associes au thread.
			mthread_create(&(threads[i]), NULL, routine, (void*)args);
			number_of_threads_remaining --;
			number_of_iterations = number_of_iterations - chunk_size_for_thread;
		}
	}

	for (i = 0; i < data->nb_thread; i++) {
		mthread_join(threads[i], NULL);
	}

	return 0;
}
