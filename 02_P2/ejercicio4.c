/**
 * @brief Sistemas Operativos: Practica 2, ejercicio 4
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del cuarto
 * ejercicio de la segunda practica, en el que se emplean hilos
 * para la multiplicacion de dos matrices por un entero.
 *
 * @file ejercicio4.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 17-03-2017
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

/**
 * @brief Parametros de entrada para la funcion mult_matrices().
 *
 * Estructura que contendra todos los datos necesarios que deberan ser
 * pasados a la funcion mult_matrices().
 */
struct param {
	int id;				/*!< Identificador del hilo en el que se esta operando */
	int mult;			/*!< Entero por el que se multiplica la matriz */
	int dim;          /*!< Tamaño de una fila/columna de la matriz */
	int *matrix;      /*!< Matriz a multiplicar */
	int fila;         /*!< Fila actual */
	struct param *h;  /*!< Estructura del otro hilo */
};

/**
 * @brief Multiplica una matriz por un entero y muestra el resultado.
 *
 * mult_matrices() recibe un puntero a void que sera una estructura (param).
 * Calculara la multiplicacion del entero por la matriz e imprimira el resultado
 * por pantalla. Tambien mostrara el estado del otro hilo, mostrando la fila
 * en la que se encuentra en sus calculos.
 *
 * @param parametros, puntero a void que contendra una estructura (param).
 */
void *mult_matrices(void *parametros);

/**
 * @brief Funcion main del ejercicio4
 *
 * El programa consiste en la creacion de 2 hilos, cada uno
 * de los cuales multiplicara una matriz por un entero, leidos ambos
 * por pantalla. Se iran imprimiendo por pantalla los resultados de
 * la multiplicacion de cada fila, asi como por que fila va el
 * otro hilo.
 *
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al reservar memoria, al introducir los argumentos de entrada
 * o al ejecutar la funcion pthread_create().
 */
int main() {
	int n, mult1, mult2;
	int *matrix1, *matrix2;
	int i;
	pthread_t h1, h2;
	struct param p1, p2;

	/* Lectura de los datos y comprobacion de su validez */
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

	/* Reserva de memoria y comprobacion */
	matrix1 = (int *) malloc(n*n * sizeof(int));
	if (!matrix1) {
		printf("Fallo en reserva de memoria.\n");
		exit(EXIT_FAILURE);
	}

	matrix2 = (int *) malloc(n*n * sizeof(int));
	if(!matrix2){
		printf("Fallo en reserva de memoria.\n");
		exit(EXIT_FAILURE);
	}

	printf("Introduzca matriz 1:\n");
	/* Lectura de la matriz */
	for (i = 0; i < n*n; i++) {
		scanf("%d", &matrix1[i]);
	}

	printf("Introduzca matriz 2:\n");
	/* Lectura de la matriz */
	for (i = 0; i < n*n; i++) {
		scanf("%d", &matrix2[i]);
	}

	/* Inicializacion de las estructuras */
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

	printf("Realizando producto:\n");

	/* Creacion de los hilos */
	if (pthread_create(&h1, NULL, mult_matrices, (void *) &p1)) {
		printf("Error al crear el hilo.\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_create(&h2, NULL, mult_matrices, (void *) &p2)) {
		printf("Error al crear el hilo.\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_join(h1, NULL)) {
		printf("Error al esperar al hilo.\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_join(h2, NULL)) {
		printf("Error al esperar al hilo.\n");
		exit(EXIT_FAILURE);
	}

	/* Liberacion de recursos */
	free(matrix1);
	free(matrix2);
}

void *mult_matrices (void *parametros) {
	struct param *p;
	int i;

	p = (struct param *) parametros;

	/* Se imprime el resultado de multiplicar el entero por cada una de las
	   filas de la matriz */
	for (p->fila = 0; p->fila < p->dim; ) {
		printf("Hilo %d multiplicando fila %d resultado ", p->id, p->fila);
		for (i = 0; i < p->dim; i++) {
			printf("%d ", p->matrix[(p->fila*p->dim)+i]*p->mult);
		}

		/* Se imprime la informacion de estado del otro hilo en la multiplicacion */
		printf(" - el Hilo %d va por la fila %d\n", p->h->id, p->h->fila);
		p->fila++;

		/* Espera de 1 segundo */
		usleep(100000);
	}

	pthread_exit(NULL);
}
