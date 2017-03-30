/**
 * @brief Sistemas Operativos: Practica 1, ejercicio 5b
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del quinto ejercicio (b) de la primera practica,
 * referente al uso de la funcion "fork" para la creacion de conjuntos de procesos de forma
 * secuencial o paralela.
 *
 * @file ejercicio5b.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 01-03-2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_PROC 3 /*!< Numero de hijos a crear */

/**
 * @brief funcion main del ejercicio5b
 *
 * Este programa genera un conjunto de procesos (NUM_PROC de hijos) de forma paralela, y para cada uno
 * imprime su ID de proceso y el ID del proceso padre.
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al ejecutar la instruccion fork().
 */
int main (void){
   pid_t pid;
   int i;

   for(i=0 ; i<NUM_PROC; i++) {
      if((pid=fork())<0) {
         printf("Error haciendo fork().\n");
         exit(EXIT_FAILURE);
      } else if (pid==0) {
         /* Cada vez que se genera un proceso hijo, este se sale del bucle y termina su ejecucion,
         de manera que el padre es el padre de todos los procesos generados */
         printf("HIJO %d, PID: %d, PPID: %d\n", i, getpid(), getppid());
         break;
      } else{
         /* Cada vez que se ejecuta el padre tras crear un hijo, espera a que termine de ejecutarse,
         de esta manera no aparece ningun proceso huerfano porque hay tantos waits como forks en el bucle */
         printf("PADRE %d, PID: %d, PPID: %d\n", i, getpid(), getppid());
         wait(NULL);
      }
   }

   exit(EXIT_SUCCESS);
}
