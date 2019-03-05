#include "mthread_internal.h"
#include <errno.h>

  /* Functions for handling conditional variables.  */

  /* Initialize condition variable COND using attributes ATTR, or use
     the default values if later is NULL.  */
int
mthread_cond_init (mthread_cond_t * __cond,
		   const mthread_condattr_t * __cond_attr)
{
 if (__cond_attr != NULL)
		not_implemented ();

	// Allocation de la liste __cond->list des threads
	__cond->list = malloc(sizeof(mthread_list_t));
	if (__cond->list == NULL) {
		perror("malloc");
		exit(errno);
	}

	// On initialise tous les éléments de la structure par des valeurs par défaut.
	__cond->list->first = NULL;
	__cond->list->last  = NULL;
	__cond->nb_thread   = 0;
	__cond->lock = 0;


	fprintf(stderr, "[COND_INIT] COND initialized\n");
	mthread_log("COND_INIT","CONDITION initialized\n");

  return 0;
}

  /* Destroy condition variable COND.  */
int
mthread_cond_destroy (mthread_cond_t * __cond)
{

	if (__cond == NULL)
		not_implemented ();

	// On prend le verrou spinlock
	mthread_spinlock_lock(&__cond->lock);
	if (__cond->nb_thread != 0) { // S'il y a toujours des threads dans la liste de threads en attente alors on retourne EBUSY
		fprintf(stderr, "[COND_DESTROY] The condition is busy, there is still some threads waiting for the condition\n");
		return EBUSY;
	}
	// Sinon on libère la mémoire.
	free(__cond->list);
	__cond->list = NULL;
	// On libère le verrou spinlock
	mthread_spinlock_unlock(&__cond->lock);

	fprintf(stderr, "[COND_DESTROY] The condition has been destroyed\n");
	mthread_log("COND_DESTROY","CONDITION destroyed\n");
	return 0;
}

  /* Wake up one thread waiting for condition variable COND.  */
int
mthread_cond_signal (mthread_cond_t * __cond)
{
	int retval = EINVAL;
	mthread_t thread_to_wake_up;
	mthread_virtual_processor_t *vp;

	if (__cond == NULL)
		return retval;

	mthread_spinlock_lock(&__cond->lock);

	if (__cond->nb_thread == 0) {
		// Rien a faire
	} else {
		if (__cond->list->first != NULL) {
			// On reveille le premier thread qui se trouve dans la liste
			thread_to_wake_up = mthread_remove_first(__cond->list);
			vp = mthread_get_vp();
			thread_to_wake_up->status = RUNNING;
			mthread_insert_last(thread_to_wake_up, &(vp->ready_list));
			//__cond->nb_thread = __cond->nb_thread - 1;
		}
	}

	mthread_spinlock_unlock(&__cond->lock);

	mthread_log("COND_SIGNAL","The condition has been signaled, a thread has been awakened\n");
	return 0;
}

  /* Wake up all threads waiting for condition variables COND.  */
int
mthread_cond_broadcast (mthread_cond_t * __cond)
{

	int retval = EINVAL;
	mthread_t* threads_to_wake_up;
	mthread_virtual_processor_t *vp;

	if (__cond == NULL)
		return retval;

	mthread_spinlock_lock(&__cond->lock);

	if (__cond->nb_thread == 0) {
		// Rien a faire
	} else {
		int i;
		threads_to_wake_up = (mthread_t*)malloc(__cond->nb_thread*sizeof(mthread_t));
		for (i=0;i<__cond->nb_thread; i++)
		{
			threads_to_wake_up[i] = mthread_remove_first(__cond->list);
			vp = mthread_get_vp();
			threads_to_wake_up[i]->status = RUNNING;
			mthread_insert_last(threads_to_wake_up[i], &(vp->ready_list));
			__cond->nb_thread = __cond->nb_thread - 1;
		}
	}

	mthread_spinlock_unlock(&__cond->lock);

	mthread_log("COND_BROADCAST","The condition has been signaled, all threads have been awakened\n");

	return 0;
}

  /* Wait for condition variable COND to be signaled or broadcast.
     MUTEX is assumed to be locked before.  */
int
mthread_cond_wait (mthread_cond_t * __cond, mthread_mutex_t * __mutex)
{
	int retval = EINVAL;
	mthread_t self;
	mthread_virtual_processor_t *vp;

	if (__cond == NULL || __mutex == NULL) {
		return retval;
	}


	mthread_spinlock_lock(&__cond->lock);


	/*if (__cond->nb_thread == 0) {
		__cond->nb_thread = 1;
		mthread_spinlock_unlock(&__cond->lock);
	} else {*/

	__cond->nb_thread++;
	self = mthread_self();
	mthread_insert_last(self,__cond->list);
	self->status = BLOCKED;
	vp = mthread_get_vp();
	vp->p = &__cond->lock;
	mthread_mutex_unlock(__mutex);
	mthread_yield();
	mthread_mutex_lock(__mutex);




	mthread_log("COND_WAIT","Waiting for the condition to be satisfied\n");
	return 0;
}
