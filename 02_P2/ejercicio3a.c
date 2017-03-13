#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_PROC 100

int isPrime(int n);

int main(int argc, char *argv[]){
	int n;
	int pid;
	int i, count;
	struct timeval before, after;

	if(argc < 2){
		printf("Se debe pasar el número de primos como argumento.\n");
		exit(EXIT_SUCCESS);
	}

	n = atoi(argv[1]);

	gettimeofday(&before, NULL);

	for(i = 0; i < MAX_PROC; i++){
		if((pid = fork()) == -1){
			printf("Error en el fork.\n");
			exit(EXIT_FAILURE);
		} else if(!pid){
			for(i = 2, count = 0; count < n; i++){
				if (isPrime(i) == 1){
					count++;
				}
			}
			exit(EXIT_SUCCESS);
		} else{
			waitpid(pid, NULL, 0);
		}
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
