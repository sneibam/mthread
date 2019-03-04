#include "mthread_internal.h"
#include <errno.h>

  /* Functions for handling semaphore.  */

int
mthread_sem_init (mthread_sem_t * sem, unsigned int value)
{
  sem->list = malloc(sizeof(mthread_list_t));
  sem->value = value;

  if (sem->list == NULL) {
	  perror("malloc");
	  exit(errno);
  }

  sem->list->first = NULL;
  sem->list->last  = NULL;

  fprintf(stderr, "[SEM_INIT] SENAPHORE initialized\n");
  return 0;
}

/* P(sem), wait(sem) */
int
mthread_sem_wait (mthread_sem_t * sem)
{
  int retval = EINVAL;
  mthread_t self;
  mthread_virtual_processor_t *vp;

  if (sem == NULL) return retval;

  mthread_spinlock_lock(&sem->lock);
  if (sem->value > 0) {
	  sem->value--;
      mthread_spinlock_unlock(&sem->lock);
  } else {
	  self = mthread_self();
	  mthread_insert_last(self, sem->list);
	  self->status = BLOCKED;
	  vp = mthread_get_vp();
	  vp->p = &sem->lock;
	  mthread_yield();
  }
  mthread_log("SEM_WAIT","SEMAPHORE DECREMENT\n");

  return retval;
}

/* V(sem), signal(sem) */
int
mthread_sem_post (mthread_sem_t * sem)
{
	int retval = EINVAL;
	mthread_t first;
	mthread_virtual_processor_t *vp;

	if (sem == NULL) return retval;

	mthread_spinlock_lock(&sem->lock);
	if (sem->list->first != NULL) {
		first = mthread_remove_first(sem->list);
		vp = mthread_get_vp();
		first->status = RUNNING;
		mthread_insert_last(first,&(vp->ready_list));
	}else {
		sem->value++;
	}


	mthread_spinlock_unlock(&sem->lock);

	mthread_log("SEM_POST","SEMAPHORE INCREMENT\n");
	return retval;
}

int
mthread_sem_getvalue (mthread_sem_t * sem, int *sval)
{
	int retval = EINVAL;
	if (sem == NULL) return retval;
	mthread_spinlock_lock(&sem->lock);
	*sval =  sem->value;
	mthread_spinlock_unlock(&sem->lock);
	return 0;
}

int
mthread_sem_trywait (mthread_sem_t * sem)
{
	int retval = EINVAL;

	if (sem == NULL) return retval;

	mthread_spinlock_lock(&sem->lock);
	if (sem->value > 0) {
	  sem->value--;
	} else {
	  mthread_log("SEM_TRY_WAIT", "SEMAPHORE TRY WAIT FAILED");
	  retval = EAGAIN;
	  goto end;
	}
	mthread_log("SEM_TRY_WAIT","SEMAPHORE DECREMENT\n");
	end:
	mthread_spinlock_unlock(&sem->lock);
	return retval;
}

/* undo sem_init() */
int
mthread_sem_destroy (mthread_sem_t * sem)
{
	if (sem == NULL)
		not_implemented ();

	mthread_spinlock_lock(&sem->lock);
	if (sem->value != 0) {
		mthread_log("SEM_DESTROY","SEMAPHORE destroyed\n");
		return EBUSY;
	}
	free(sem->list);
	sem->list = NULL;
	mthread_spinlock_unlock(&sem->lock);

	mthread_log("SEM_DESTROY","SEMAPHORE destroyed\n");
	return 0;
}
