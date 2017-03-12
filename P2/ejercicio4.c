
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

void *mult_matrices(void *parametros);

struct param{
	int id;
	int mult;
	int dim;
	int *matrix;
	int fila;
	struct param *h;
};

int main(){
	int n, mult1, mult2;
	int *matrix1, *matrix2;
	int i;
	pthread_t h1, h2;
	struct param p1, p2;

	printf("Introduzca dimensión de la matriz cuadrada:\n");
	scanf("%d", &n);	
	if(n > 4){
		printf("La dimensión de la matriz no puede exceder de 4.\n");
		exit(EXIT_FAILURE);
	}
	printf("Introduzca multiplicador 1:\n");
	scanf("%d", &mult1);
	printf("Introduzca multiplicador 2:\n");
	scanf("%d", &mult2);
	printf("Introduzca matriz 1:\n");
	matrix1 = (int *) malloc(n*n * sizeof(int));
	for(i = 0; i < n*n; i++){
		scanf("%d", &matrix1[i]);
	}
	printf("Introduzca matriz 2:\n");
	matrix2 = (int *) malloc(n*n * sizeof(int));
	for(i = 0; i < n*n; i++){
		scanf("%d", &matrix2[i]);
	}
	printf("Realizando producto:\n");

	p1.id = 1;
	p1.mult = mult1;
	p1.dim = n;
	p1.matrix = matrix1;
	p1.fila = 0;
	p2.id = 2;
	p2.mult = mult2;
	p2.dim = n;
	p2.matrix = matrix2;
	p2.fila = 0;

	p1.h = &p2;
	p2.h = &p1;

	pthread_create(&h1, NULL, mult_matrices, (void *) &p1);
	pthread_create(&h2, NULL, mult_matrices, (void *) &p2);	

	pthread_join(h1, NULL);
	pthread_join(h2, NULL);

	free(matrix1);
	free(matrix2);
}

void *mult_matrices(void *parametros){
	struct param *p;
	int i;
	p = (struct param *) parametros;

	for(p->fila = 0; p->fila < p->dim; ){
		printf("Hilo %d multiplicando fila %d resultado ", p->id, p->fila);
		for(i = 0; i < p->dim; i++){
			printf("%d ", p->matrix[(p->fila*p->dim)+i]*p->mult);
		}
		printf(" - el Hilo %d va por la fila %d ", p->h->id, p->h->fila);
		printf("\n");
		p->fila++;
		usleep(100000);
	}
}