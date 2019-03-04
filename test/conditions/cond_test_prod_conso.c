#include <stdio.h>
#include <pthread.h>

/* Producteur/Consommateur Test */

volatile char theChar = '\0';
volatile int affiche = 1;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_lire = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *lire(void* name)
{
	do{
		while(affiche==1) printf("Waiting\n");
		pthread_mutex_lock(&lock);
		printf("Le producteur prend le mutex\n");
		affiche = 1;
		theChar = getchar();
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&lock);
		pthread_cond_wait(&cond_lire, &lock);
		printf("Le producteur prend le mutex\n");
	} while (theChar != 'F');
	return NULL;
}

void *affichage(void* name)
{
	do {
		pthread_mutex_lock(&lock);
		affiche = 0;
		printf("Le Consommateur prend le mutex\n");
		pthread_cond_wait(&cond, &lock);
		printf("car = %c\n", theChar);
		pthread_cond_signal(&cond_lire);
		pthread_mutex_unlock(&lock);
		printf("Le Consommateur libere le mutex\n");
	} while(theChar != 'F');
	return NULL;
}

int main(int argc, char** argv)
{

	pthread_t producteur;
	pthread_create(&(producteur), NULL, lire, NULL);
	pthread_t consommateur;
	pthread_create(&(consommateur), NULL, affichage, NULL);
	pthread_join(producteur, NULL);
	pthread_join(consommateur, NULL);

	return 0;

}
