/**
 * @brief Sistemas Operativos: Practica 2, ejercicio 3a
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del primer apartado del tercer
 * ejercicio de la segunda practica, que consiste en la comparacion de
 * tiempos entre hilos y procesos.
 *
 * @file ejercicio3a.c
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

#define MAX_PROC 100 /*!< Maximo de proccesos a crear */

/**
 * @brief Comprueba si un numero es primo.
 *
 * isPrime() recibe un entero, comprueba si este es primo y devuelve el resultado.
 * @param n, entero que se quiere comprobar.
 * @return 0 si el numero no es primo, 1 si el numero es primo.
 */
int isPrime (int n);

/**
 * @brief Funcion main del ejercicio3a
 *
 * El programa consiste en la creacion de 100 procesos hijos en serie, cada uno
 * de los cuales calculara N numeros primos. El proceso
 * padre esperara a cada uno de sus hijos, y una vez hayan finalizado
 * todos los hijos se imprimira por pantalla el tiempo total empleado en
 * todos los calculos.
 *
 * @param argc, numero de argumentos
 * @param argv, array de strings con los argumentos (numero de primos a calcular).
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al introducir los argumentos de entrada
 * o al ejecutar la funcion fork().
 */
int main (int argc, char *argv[]) {
   int n, pid, i, count;
	struct timeval before, after;

 	/* Comprobación de los argumentos de entrada */
	if(argc != 2){
		printf("Se debe pasar el número de primos como único argumento.\n");
		exit(EXIT_FAILURE);
	}

	n = atoi(argv[1]);

	/* Tiempo inicial */
	gettimeofday(&before, NULL);

	for(i = 0; i < MAX_PROC; i++){
		if((pid = fork()) == -1){
			printf("Error en el fork.\n");
			exit(EXIT_FAILURE);
		} else if(!pid){
			/* Cada hijo calcula los n primos */
			for(i = 2, count = 0; count < n; i++){
				if (isPrime(i) == 1){
					count++;
				}
			}
			exit(EXIT_SUCCESS);
		} else{
			/* El padre espera la terminacion de todos los hijos */
			waitpid(pid, NULL, 0);
		}
	}

	/* Tiempo final */
	gettimeofday(&after, NULL);

	printf("Tiempo empleado: %ld usecs.\n", (after.tv_sec - before.tv_sec)*(10^6) + (after.tv_usec - before.tv_usec));

	exit(EXIT_SUCCESS);

}

int isPrime (int n) {
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
