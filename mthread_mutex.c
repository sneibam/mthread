#include <errno.h>
#include "mthread_internal.h"



  /* Functions for mutex handling.  */

  /* Initialize MUTEX using attributes in *MUTEX_ATTR, or use the
     default values if later is NULL.  */
int
mthread_mutex_init (mthread_mutex_t * __mutex,
                    const mthread_mutexattr_t * __mutex_attr) {
    if (__mutex_attr != NULL)
        not_implemented ();

    // Initialisation par defaut de tout les elements de la structure mthread_mutex_t
    __mutex->list = NULL;
    __mutex->lock = 0;
    __mutex->nb_thread  = 0;

    fprintf(stderr, "[MUTEX_INIT] MUTEX initialized\n");
    return 0;
}

/* Destroy MUTEX.  */
int
mthread_mutex_destroy (mthread_mutex_t * __mutex) {
    if (__mutex == NULL) {
        not_implemented ();
    }

    mthread_spinlock_lock(&__mutex->lock); // On prend le verrou spinlock

    if (__mutex->nb_thread != 0) { // S'il y a un thread qui est toujours dans la liste d'attente, on return EBUSY
        return EBUSY;
    }
    if (__mutex->list) {
        free(__mutex->list); // Sinon on libère la liste si celle-ci existe
        __mutex->list = NULL;
    }

    mthread_spinlock_unlock(&__mutex->lock); // On relache le verrou

    fprintf(stderr, "[MUTEX_DESTROY] MUTEX destroyed\n");
    mthread_log("MUTEX_DESTROY","MUTEX destroyed\n");
    return 0;
}

/* Try to lock MUTEX.  */
int
mthread_mutex_trylock (mthread_mutex_t * __mutex) {

	mthread_log("MUTEX_TRY_LOCK","Trying to acquire the mutex\n");
	fprintf(stderr, "[MUTEX_TRY_LOCK] Trying to acquire the mutex\n");


	int retval = EINVAL;
	/*mthread_t self;
	mthread_virtual_processor_t *vp;*/

	if (__mutex == NULL)
        return retval;

    mthread_spinlock_lock(&__mutex->lock); // On prend le verrou

    if (__mutex->nb_thread == 0) // Si il n'y aucun thread qui attend et donc le mutex est libre, alors on attribue le mutex au thread.
	{
		__mutex->nb_thread = 1;
		mthread_spinlock_unlock(&__mutex->lock);
		mthread_log("MUTEX_TRY_LOCK","MUTEX ACQUIRED\n");
		fprintf(stderr, "[MUTEX_TRY_LOCK] MUTEX ACQUIRED\n");
		return 1; // On retourne 1 Si le mutex a ete acquis
	}
	// Sinon le thread ne se bloque pas car il s'agit uniquement d'un try_lock.
	mthread_spinlock_unlock(&__mutex->lock); // On libère le verrou.

	fprintf(stderr, "[MUTEX_TRY_LOCK] MUTEX NOT ACQUIRED\n");
    mthread_log("MUTEX_TRY_LOCK","MUTEX NOT ACQUIRED\n");

    return 0; // On retourne 0 si le mutex n'a pas ete acquis
}

/* Wait until lock for MUTEX becomes available and lock it.  */
int
mthread_mutex_lock (mthread_mutex_t * __mutex) {

	int retval = EINVAL;
	mthread_t self;
	mthread_virtual_processor_t *vp;

    if (__mutex == NULL)
        return retval;

    mthread_spinlock_lock(&__mutex->lock); // On prend le verrou mutex_lock



    if (__mutex->nb_thread == 0) { // S'il n'y a aucun thread qui attend, on attribue le mutex au thread

        __mutex->nb_thread = 1;
		mthread_spinlock_unlock(&__mutex->lock);
		mthread_log("MUTEX_LOCK","MUTEX ACQUIRED\n");

    } else {
    	// Sinon on met le thread à l'etat 'BLOCKED' et on met ajoute le thread a la fin de la liste des threads en attente.
		if (__mutex->list == NULL) { // On alloue de la memoire pour la liste des threads si necessaire
			__mutex->list = malloc(sizeof(mthread_list_t));
			if (__mutex->list == NULL) {
				perror("malloc");
				exit(errno);
			}
			__mutex->list->first = NULL;
			__mutex->list->last  = NULL;
		}
		// On recupere le thread courant
		self = mthread_self();
		// On le  rajoute dans la liste des threads en attente de la liberation du mutex
		mthread_insert_last(self,__mutex->list);
		// On met le thread a l'etat 'BLOCKED'
		self->status = BLOCKED;

		vp = mthread_get_vp();
		vp->p = &__mutex->lock;

		//On passe au thread suivant en appelant l'ordonnanceur
		mthread_yield();

		mthread_log("MUTEX_LOCK","MUTEX is locked\n");

		return EDEADLK;
    }

    return 0;
}

/* Unlock MUTEX.  */
int
mthread_mutex_unlock (mthread_mutex_t * __mutex) {

	int retval = EINVAL;
	mthread_t first;
	mthread_virtual_processor_t *vp;

    if (__mutex == NULL)
        return retval;

    mthread_spinlock_lock(&__mutex->lock); // On prend le verrou spinlock


    if (__mutex->nb_thread == 0) { // Si le mutex n'est pas pris on retourne Operation interdite (EPERM)
        mthread_spinlock_unlock(&__mutex->lock);
        mthread_log("MUTEX_UNLOCK","MUTEX wasnt locked\n");
        return EPERM;
    }

    if (__mutex->list != NULL) { // S'il y a bien un thread dans la liste des threads en attente du mutex

    	// On retire le premier afin de le debloquer
        first = mthread_remove_first(__mutex->list);


        if (__mutex->list->first == NULL) { // Si on a retirer le dernier thread alors on libere l'espace memoire
            free(__mutex->list);
            __mutex->list = NULL;
        }

        // On met le thread a l'etat 'RUNNING'
        first->status = RUNNING;

        /** ce thread devient prêt à l'execution */
        vp = mthread_get_vp();
        mthread_insert_last(first, &(vp->ready_list));
    } else {
        __mutex->nb_thread = 0;
    }

    mthread_spinlock_unlock(&__mutex->lock);

    mthread_log("MUTEX_UNLOCK","MUTEX released\n");
    return 0;
}
