#include <errno.h>
#include "mthread_internal.h"



  /* Functions for mutex handling.  */

  /* Initialize MUTEX using attributes in *MUTEX_ATTR, or use the
     default values if later is NULL.  */
int
mthread_mutex_init (mthread_mutex_t * __mutex,
		    const mthread_mutexattr_t * __mutex_attr)
{
    if (__mutex_attr != NULL) 
        not_implemented ();

    // Allocation de la liste des threads en attente
    __mutex->list = malloc(sizeof(mthread_list_t));
    if (__mutex->list == NULL) {
        perror("malloc");
        exit(errno);
    }
    // Initialisation par defaut de tout les elements de la structure mthread_mutex_t

    __mutex->list->first = NULL;
    __mutex->list->last  = NULL;
    __mutex->nb_thread   = 0;

    fprintf(stderr, "[MUTEX_INIT] MUTEX initialized\n");
    mthread_log("MUTEX_INIT","MUTEX initialized\n");

    return 0;
}

  /* Destroy MUTEX.  */
int
mthread_mutex_destroy (mthread_mutex_t * __mutex)
{
    if (__mutex == NULL) 
        not_implemented ();

    mthread_spinlock_lock(&__mutex->lock); // On prend le verrou
    if (__mutex->nb_thread != 0) // S'il y a un thread qui est toujours dans la liste d'attente, on return EBUSY
        return EBUSY;
    free(__mutex->list); // Sinon on libère la liste.
    __mutex->list = NULL;
    mthread_spinlock_unlock(&__mutex->lock); // On lache le verro.u

    fprintf(stderr, "[MUTEX_DESTROY] MUTEX destroyed\n");
    mthread_log("MUTEX_DESTROY","MUTEX destroyed\n");
    return 0;
}

  /* Try to lock MUTEX.  */
int
mthread_mutex_trylock (mthread_mutex_t * __mutex)
{
	mthread_log("MUTEX_TRY_LOCK","Trying to acquire the mutex\n");
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
    	return 1;
    }
    // Sinon le thread ne se bloque pas car il s'agit uniquement d'un try_lock.
    mthread_spinlock_unlock(&__mutex->lock); // On libère le verrou.
    mthread_log("MUTEX_TRY_LOCK","MUTEX NOT ACQUIRED\n");
    return 0;
}

  /* Wait until lock for MUTEX becomes available and lock it.  */
int
mthread_mutex_lock (mthread_mutex_t * __mutex)
{
    int retval = EINVAL;
    mthread_t self;
    mthread_virtual_processor_t *vp;

    if (__mutex == NULL) 
        return retval;

    mthread_spinlock_lock(&__mutex->lock); // On prend le verrou mutex_lock

    if (__mutex->nb_thread == 0) { // S'il n'y a aucun thread qui attend, on attribue le mutex au thread
        __mutex->nb_thread = 1;
        mthread_spinlock_unlock(&__mutex->lock);
    } else { // Sinon on met le thread à l'etat 'BLOCKED' et on met ajoute le thread a la fin de la liste des threads en attente.
        self = mthread_self();
        mthread_insert_last(self,__mutex->list);
        self->status = BLOCKED;
        vp = mthread_get_vp();
        vp->p = &__mutex->lock;
        mthread_yield();
    }


    mthread_log("MUTEX_LOCK","MUTEX acquired\n");
    return retval;
}

  /* Unlock MUTEX.  */
int
mthread_mutex_unlock (mthread_mutex_t * __mutex)
{
    int retval = EINVAL;
    mthread_t first;
    mthread_virtual_processor_t *vp;

    if (__mutex == NULL) 
        return retval;

    // On prend le verrou
    mthread_spinlock_lock(&__mutex->lock);
    // On reveille le premier thread dans la liste des threads en attente si jamais il y a un thread dans la liste.
    if (__mutex->list->first != NULL) { 
        // On retire le premier thread de la liste __mutex->list
        first = mthread_remove_first(__mutex->list);
        // On recupere le virtual processor
        vp = mthread_get_vp();
        // On met le thread à reveiller à l'état 'RUNNING'
        first->status = RUNNING;
        // On met le thread dans la liste des threads prets à etre executer.
        mthread_insert_last(first,&(vp->ready_list));
    } else {
        /* S'il n'y a aucun thread dans la liste des threads bloqué, 
            alors on remet juste la valeur de nb_thread à 0, pour indiquer qu'il n'y aucun thread qui a pris le mutex. */
        __mutex->nb_thread = 0;
    }

    // On libère le verrou __mutex->lock;
    mthread_spinlock_unlock(&__mutex->lock);
    
    mthread_log("MUTEX_UNLOCK","MUTEX released\n");
    return retval;
}
