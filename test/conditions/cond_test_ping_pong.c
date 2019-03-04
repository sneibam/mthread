#include <stdio.h>
#include <pthread.h>

/* Ping/Pong Test */

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_ping = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_pong = PTHREAD_COND_INITIALIZER;

void* ping(void* arg)
{
	while(1)
	{
		pthread_mutex_lock(&lock);
		printf("PING\n");
		pthread_cond_signal(&cond_pong);
		pthread_cond_wait(&cond_ping, &lock);
		pthread_mutex_unlock(&lock);

	}
	return NULL;
}


void* pong(void* arg)
{
	while(1)
	{
		pthread_mutex_lock(&lock);
		pthread_cond_wait(&cond_pong, &lock);
		printf("PONG\n");
		pthread_cond_signal(&cond_ping);
		pthread_mutex_unlock(&lock);
	}
	return NULL;
}
/*void *lire(void* name)
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
		printf("Le producteur libere le mutex\n");
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
}*/

int main(int argc, char** argv)
{

	pthread_t A, B;
	pthread_create(&(A), NULL, ping, NULL);
	pthread_create(&(B), NULL, pong, NULL);
	pthread_join(A, NULL);
	pthread_join(B, NULL);


	return 0;

}
