/**
 * @brief Sistemas Operativos: Practica 2, ejercicio 3b
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del segundo apartado del tercer
 * ejercicio de la segunda practica, que consiste en la comparacion de
 * tiempos entre hilos y procesos.
 *
 * @file ejercicio3b.c
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

#define MAX_HILOS 100 /*!< Maximo de hilos a crear */

/**
 * @brief Comprueba si un numero es primo.
 *
 * isPrime() recibe un entero, comprueba si este es primo y devuelve el resultado.
 * @param n, entero que se quiere comprobar.
 * @return 0 si el numero no es primo, 1 si el numero es primo.
 */
int is_prime (int n);

/**
 * @brief Calcula los primeros n primos.
 *
 * primos() recibe un entero que indica el numero de primos a generar
 * y calcula los mismos.
 * @param n, numero de primos a generar.
 * @return puntero a void del numero de primos a generar.
 */
void *primos (void * n);

/**
 * @brief Funcion main del ejercicio3b
 *
 * El programa consiste en la creacion de 100 hilos, cada uno
 * de los cuales calculara N numeros primos. Una vez hayan finalizado
 * todos los hilos se imprimira por pantalla el tiempo total empleado en
 * todos los calculos.
 *
 * @param argc, numero de argumentos
 * @param argv, array de strings con los argumentos (numero de primos a calcular).
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al introducir los argumentos de entrada
 * o al ejecutar la funcion pthread_create().
 */
int main (int argc, char *argv[]) {
	int n, i;
	struct timeval before, after;
	pthread_t h[MAX_HILOS];

	/* Comprobacion de los argumentos de entrada */
	if(argc != 2){
		printf("Se debe pasar el número de primos como argumento.\n");
		exit(EXIT_FAILURE);
	}

	n = atoi(argv[1]);

	/* Tiempo inicial */
	gettimeofday(&before, NULL);

	/* Creacion de los hilos */
	for (i = 0; i < MAX_HILOS; i++) {
		if (pthread_create(&h[i], NULL, primos, (void *) &n)) {
			printf("Error al crear el hilo.\n");
			exit(EXIT_FAILURE);
		}
		if (pthread_join(h[i], NULL)) {
			printf("Error al esperar al hilo.\n");
			exit(EXIT_FAILURE);
		}
	}

	/* Tiempo final */
	gettimeofday(&after, NULL);

	printf("Tiempo empleado: %ld usecs.\n", (after.tv_sec - before.tv_sec)*(10^6) + (after.tv_usec - before.tv_usec));

	exit(EXIT_SUCCESS);

}

int is_prime (int n) {
	int j;

	if (n <= 1) {
		return 0;
	} else if (n == 2) {
		return 1;
	}

	for (j = 2; j*j <= n; j++) {
		if (n%j == 0) {
			return 0;
		}
	}

	return 1;
}

void *primos(void * n){
	int i, count;
	int num = *((int *) n);

	for (i = 2, count = 0; count < num; i++) {
		if (is_prime(i) == 1) {
			count++;
		}
	}

	return n;
}
