/**
 * @brief Sistemas Operativos: Practica 2, ejercicio 8
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del octavo
 * ejercicio de la segunda practica, que consiste en el envío cíclico
 * de señales entre procesos creados se forma secuencial.
 *
 * @file ejercicio8.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 17-03-2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @brief Imprime fecha y hora.
 *
 * impr_tiempo() imprime la fecha y hora del momento en el que es llamada.
 */
void impr_tiempo();

/**
 * @brief Manejador principal
 *
 * Este manejador se encarga de recoger SIGUSR1 o SIGTERM y actuar como
 * se pide en el ejercicio, excluyendo la parte de terminar el proceso.
 * @param sig, numero de la señal
 */
void manejador(int sig);

/**
 * @brief Manejador secundario
 *
 * Manejador vacio. Sirve para cuando se han acabado las v vueltas y
 * necesitamos que el ultimo hijo informe al padre sin que el padre
 * imprima de nuevo por pantalla, antes de empezar a terminar el resto
 * de procesos con SIGTERM.
 * @param sig, numero de la señal
 */
void manejador2(int sig);

/**
 * @brief Funcion main del ejercicio8
 *
 * El programa consiste en la creacion de n procesos secuencialmente.
 * Una vez creados, el ultimo hijo mandara una señal SIGUSR1 al primer padre,
 * este la mandara a su hijo, y asi sucesivamente, hasta que se llegue a un numero
 * v de vueltas realizadas. Cuando esto ocurra, el primer padre
 * mandara una señal SIGTERM a su hijo, que se propagara hasta llegar al
 * ultimo hijo.
 *
 * @param argc, numero de argumentos
 * @param argv, array de strings con los argumentos (numero de procesos y numero de vueltas).
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al introducir los argumentos de entrada, al asignar los
 * manejadores o al ejecutar la funcion fork().
 */
int main(int argc, char *argv[]){

	int n, v;
	int i;
	int ppid, pid;

	/* Manejadores a utilizar */
	void manejador();
	void manejador2();

	/* Comprobacion de los argumentos de entrada */
	if(argc != 3){
		printf("Se debe pasar el número de procesos y vueltas como argumento.\n");
		exit(EXIT_FAILURE);
	}

	/* Asignacion de los manejadores */
	signal(SIGTERM, &manejador);
   signal(SIGUSR1, &manejador);

   /* Lectura del numero de procesos y el numero de vueltas */
   n = atoi(argv[1]);
	v = atoi(argv[2]);

	/* PID del proceso padre raiz */
	ppid = getpid();

	for(i = 0; i < n; i++){
		if((pid = fork()) == -1){
			printf("Error en el fork().\n");
			exit(EXIT_FAILURE);
		} else if(pid){
			/* Se hacen v pause() y cuando se recibe la señal
			se manda al proceso hijo */
			while(v){
				pause();
				kill(pid, SIGUSR1);
				v--;
			}
			/* El primer padre tras completar las v vueltas
			cambiara el manejador a uno vacio, esperara a la llegada
			de otra señal y mandara una SIGTERM a su hijo. Cuando el
			la reciba, acabara */
			if(i == 0){
				signal(SIGUSR1, &manejador2);
				pause();
				sleep(1);
				kill(pid, SIGTERM);
				pause();
				exit(EXIT_SUCCESS);
			}
			/* Los demas padres esperaran una señal SIGTERM */
			pause();
			kill(pid, SIGTERM);
			break;
		} else{
			;
		}
	}

	/* El ultimo hijo cumple las v vueltas mandando la señal SIGUSR1
	al primer padre */
	if(i == n){
		while(v){
			sleep(5);
			kill(ppid, SIGUSR1);
			v--;
			pause();
		}
		kill(ppid, SIGUSR1);
		/* Cuando recibe la señal SIGTERM, se la devuelve al primer padre */
		pause();
		kill(ppid, SIGTERM);
	}
	exit(EXIT_SUCCESS);

}

void manejador(int sig){
	if(sig == SIGUSR1){
		printf("Hola PID=%d. ", getpid());
		impr_tiempo();
		fflush(stdout);
		sleep(2);
	} else if(sig == SIGTERM){
		sleep(1);
		printf("Muere PID=%d.\n", getpid());
	}
}

void manejador2(int sig){
	return;
}

void impr_tiempo() {
	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	char output[128];
	strftime(output, 128, "%d/%m/%y %H:%M:%S", tlocal);
	printf("Fecha y Hora: %s\n",output);
}
