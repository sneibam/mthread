#include "mthread_internal.h"
#include <errno.h>

  /* Functions for handling semaphore.  */

int
mthread_sem_init (mthread_sem_t * sem, unsigned int value)
{
  // Allocation memoire de la liste sem->list qui va contenir la liste des threads en attente
  sem->list = malloc(sizeof(mthread_list_t));
  // Initialisation de la valeur initiale du semaphore. Cette valeur correspond au nombre maximum de threads qui peuvent entrer dans la section critique
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

  // Si le semaphore vaut null, on retourne invalid argument.
  if (sem == NULL) return retval;

  // On prend le verrou spinlock.
  mthread_spinlock_lock(&sem->lock);
  // Si la valeur du semaphore est > a 0, donc on peut permettre au thread d'enter dans la section critique
  if (sem->value > 0) {
	  sem->value--;
      mthread_spinlock_unlock(&sem->lock);
  } else { // Sinon on bloque le thread et on le met dans la liste des threads en attente.
	  self = mthread_self();
	  mthread_insert_last(self, sem->list);
	  self->status = BLOCKED;
	  vp = mthread_get_vp();
	  vp->p = &sem->lock;
	  mthread_yield(); // On appelle l'ordonnanceur pour qu'il donne la main a un autre thread
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

	// Si le semaphore vaut null, on retourne invalid argument.
	if (sem == NULL) return retval;

	// On prend le verrou spinlock
	mthread_spinlock_lock(&sem->lock);
	if (sem->list->first != NULL) { // On retire le premier thread qui se trouve dans la liste des threads en attente et on le debloque.
		first = mthread_remove_first(sem->list); // On recupere le thread qui etaient bloques en on le retire de la liste
		vp = mthread_get_vp(); // On recupere le processur virtuel courant.
		first->status = RUNNING; // On met le thread en question a l'etat 'RUNNING'
		mthread_insert_last(first,&(vp->ready_list)); // Et enfin on le rajoute dans liste des threads prets de l'ordonnanceur
	}else {
		sem->value++; // Sinon on incremente juste la valeur du semaphore.
	}


	mthread_spinlock_unlock(&sem->lock); // On libere le verrou spinlock

	mthread_log("SEM_POST","SEMAPHORE INCREMENT\n");
	return 0;
}

int
mthread_sem_getvalue (mthread_sem_t * sem, int *sval)
{
	int retval = EINVAL;
	if (sem == NULL) return retval;
	mthread_spinlock_lock(&sem->lock);
	*sval =  sem->value; // On recupere juste la valeur courante du semaphore.
	mthread_spinlock_unlock(&sem->lock);
	return 0;
}

int
mthread_sem_trywait (mthread_sem_t * sem)
{
	int retval = EINVAL;
	// Si le semaphore vaut null, on retoure invalid argument
	if (sem == NULL) return retval;

	// On prend le verrou spinlock
	mthread_spinlock_lock(&sem->lock);
	// Si la valeur de semaphore est > 0, on decremente la valeur du semaphore, et on laisse le thread acceder a la section critique
	if (sem->value > 0) {
	  sem->value--;
	} else { // Sinon on affiche ce message de log sans bloquer le thread.
	  mthread_log("SEM_TRY_WAIT", "SEMAPHORE TRY WAIT FAILED");
	  retval = EAGAIN;
	  goto end;
	}
	mthread_log("SEM_TRY_WAIT","SEMAPHORE DECREMENT\n");
	end:
	mthread_spinlock_unlock(&sem->lock); // On libere le verrou spinlock
	return retval;
}

/* undo sem_init() */
int
mthread_sem_destroy (mthread_sem_t * sem)
{
	if (sem == NULL)
		not_implemented ();

	// On prend le verrou spinlock
	mthread_spinlock_lock(&sem->lock);
	if (sem->list->first != NULL) { // Si il reste encore des threads en attente du semaphore alors on ne detruit pas celui-ci.
		mthread_log("SEM_DESTROY","SEMAPHORE is still busy\n");
		mthread_spinlock_unlock(&sem->lock);
		return EBUSY;
	}
	free(sem->list);
	sem->list = NULL;
	mthread_spinlock_unlock(&sem->lock);

	mthread_log("SEM_DESTROY","SEMAPHORE destroyed\n");
	return 0;
}
