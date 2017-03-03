/**
 * @brief Sistemas Operativos: Practica 1, ejercicio 5a
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del quinto ejercicio (a) de la primera practica,
 * referente al uso de la funcion "fork" para la creacion de conjuntos de procesos de forma
 * secuencial o paralela.
 *
 * @file ejercicio5a.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 01-03-2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define NUM_PROC 3 /*!< Numero de hijos a crear */

/**
 * @brief funcion main del ejercicio5a
 *
 * Este programa genera un conjunto de procesos (NUM_PROC de hijos) secuencialmente, y para cada uno
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
         printf("HIJO %d, PID: %d, PPID: %d\n", i, getpid(), getppid());
      } else{
         /* Cada vez que se itera el bucle y se crea un hijo, el padre sale del bucle y no
         crea ningun proceso hijo mas */
         printf("PADRE %d, PID: %d, PPID: %d\n", i, getpid(), getppid());
         break;
      }
   }

   /* Al final de cada main de cada proceso, se ejecuta wait() para que todos los procesos padre
   esperen a que los procesos hijos terminen (el proceso que no es padre de ninguno no se ve
   afectado por el wait). */
   wait(NULL);
   exit(EXIT_SUCCESS);
}
