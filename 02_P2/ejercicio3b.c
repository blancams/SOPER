
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_HILOS 2

int isPrime(int n);
void *primos(void * n);

int main(int argc, char *argv[]){
	int n, i;
	struct timeval before, after;
	pthread_t h;

	if(argc < 2){
		printf("Se debe pasar el número de primos como argumento.\n");
		exit(EXIT_SUCCESS);
	}

	n = atoi(argv[1]);

	gettimeofday(&before, NULL);

	for(i = 0; i < MAX_HILOS; i++){
		pthread_create(&h, NULL, primos, (void *) &n);
		pthread_join(h, NULL);
	}

	gettimeofday(&after, NULL);

	printf("Tiempo empleado: %ld usecs.\n", (after.tv_sec - before.tv_sec)*(10^6) + (after.tv_usec - before.tv_usec));

	exit(EXIT_SUCCESS);

}

int isPrime(int n){
	int j;
	for(j = 2; j*j <= n; j++){
		if(n%j == 0){
			return 0;
		}
	}
	return 1;
}

void *primos(void * n){
	int i, count;
	int num = *((int *) n);
	for(i = 2, count = 0; count < num; i++){
		if(isPrime(i) == 1){
			count++;
		}
	}

	return n;
}
