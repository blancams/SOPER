/**
 * @brief Sistemas Operativos: Practica 1, ejercicio 6
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del sexto ejercicio de la primera practica,
 * referente al estudio de las consecuencias e implicaciones de la creacion de procesos hijos,
 * en este caso en lo relacionado a recursos.
 *
 * @file ejercicio6.c
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

#define STRING_L 80 /*!< Longitud maxima de la cadena de caracteres */

/**
 * @brief funcion main del ejercicio6
 *
 * Este programa reserva memoria para una cadena de caracteres y efectua un fork()
 * a continuacion. EL proceso hijo escribe un valor en la cadena pasado por pantalla.
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al ejecutar la instruccion fork() o en la reserva
 * de memoria.
 */
int main (void){
   pid_t pid;
   char *string;

   /* Reserva de memoria y comprobacion de errores */
   string = (char*) malloc (STRING_L*sizeof(char));
   if(string == NULL){
      exit(EXIT_FAILURE);
   }

   /* Creacion del proceso hijo y comprobacion del fork() */
   if ((pid=fork()) < 0) {
      printf("Error haciendo fork().\n");
      exit(EXIT_FAILURE);
   } else if (pid == 0) {
      /* Codigo del hijo */
      fprintf(stdout, "HIJO %d\n", getpid());
      /* Escritura en la cadena de caracteres de un valor por pantalla */
      fprintf(stdout, "Introduce un nombre: ");
      fscanf(stdin, "%s", string);
   } else {
      /* Codigo del padre */
      fprintf(stdout, "PADRE %d\n", getpid());
   }

   /* El padre espera a que el hijo termine */
   wait(NULL);
   /* Escritura de los valores de las cadenas para padre e hijo y liberacion de memoria */
   fprintf(stdout, "El nombre introducido para %d es %s\n", getpid(), string);
   free(string);

   exit(EXIT_SUCCESS);
}
