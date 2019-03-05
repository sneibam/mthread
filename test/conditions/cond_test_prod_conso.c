#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "../mthread.h"

/* Producteur/Consommateur Test */

volatile char theChar = '\0';
volatile int affiche = 1;
mthread_mutex_t lock;
mthread_cond_t cond_lire;
mthread_cond_t cond;

void *lire(void* name)
{
	sleep(5); // Synchronisation
	do{
		while(affiche==1) printf("Waiting\n");
		mthread_mutex_lock(&lock);
		affiche = 1;
		printf("Veuillez entrer un caractere: ");
		scanf("%c",&theChar);
		getchar();
		mthread_cond_signal(&cond);
		mthread_cond_wait(&cond_lire, &lock);
		mthread_mutex_unlock(&lock);
	} while (theChar != 'F');
	return NULL;
}

void *affichage(void* name)
{
	do {
		mthread_mutex_lock(&lock);
		affiche = 0;
		mthread_cond_wait(&cond, &lock);
		printf("car = %c\n", theChar);
		mthread_cond_signal(&cond_lire);
		mthread_mutex_unlock(&lock);
	} while(theChar != 'F');
	return NULL;
}

int main(int argc, char** argv)
{

	// Initialisation de mutex et des conditions
	mthread_cond_init(&cond, NULL);
	mthread_cond_init(&cond_lire, NULL);
	mthread_mutex_init(&lock, NULL);

	mthread_t producteur;
	mthread_create(&(producteur), NULL, lire, NULL);
	mthread_t consommateur;
	mthread_create(&(consommateur), NULL, affichage, NULL);
	mthread_join(producteur, NULL);
	mthread_join(consommateur, NULL);

	return 0;

}
