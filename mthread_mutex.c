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

    return 0;
}

  /* Destroy MUTEX.  */
int
mthread_mutex_destroy (mthread_mutex_t * __mutex)
{
    if (__mutex == NULL) 
        not_implemented ();

    mthread_spinlock_lock(&__mutex->lock); // On prend le verrou
    if (__mutex->nb_thread != 0)
        return EBUSY;
    free(__mutex->list);
    __mutex->list = NULL;
    mthread_spinlock_unlock(&__mutex->lock);

    fprintf(stderr, "[MUTEX_DESTROY] MUTEX destroyed\n"); // (AJOUT)
    mthread_log("MUTEX_DESTROY","MUTEX destroyed\n");
    return 0;
}

  /* Try to lock MUTEX.  */
int
mthread_mutex_trylock (mthread_mutex_t * __mutex)
{
	/*Ajout: Implementation de la fonction try_lock*/
	mthread_log("MUTEX_TRY_LOCK","Trying to acquire the mutex\n");
	int retval = EINVAL;
	/*mthread_t self;
	mthread_virtual_processor_t *vp;*/

	if (__mutex == NULL)
		return retval;

    mthread_spinlock_lock(&__mutex->lock);

    if (__mutex->nb_thread == 0)
    {
    	__mutex->nb_thread = 1;
    	mthread_spinlock_unlock(&__mutex->lock);
    	mthread_log("MUTEX_TRY_LOCK","MUTEX ACQUIRED\n");
    	return 1;
    }
    mthread_spinlock_unlock(&__mutex->lock);
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

    mthread_spinlock_lock(&__mutex->lock);

    if (__mutex->nb_thread == 0) {
        __mutex->nb_thread = 1;
        mthread_spinlock_unlock(&__mutex->lock);
    } else {
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

    mthread_spinlock_lock(&__mutex->lock);
    if (__mutex->list->first != NULL) {
        first = mthread_remove_first(__mutex->list);
        vp = mthread_get_vp();
        first->status = RUNNING;
        mthread_insert_last(first,&(vp->ready_list));
    } else {
        __mutex->nb_thread = 0;
    }


    mthread_spinlock_unlock(&__mutex->lock);
    
    mthread_log("MUTEX_UNLOCK","MUTEX released\n");
    return retval;
}
