/**
 * @brief Sistemas Operativos: Practica 2, ejercicio 6
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del sexto
 * ejercicio de la segunda practica, en el que se emplean señales
 * para la terminacion de procesos.
 *
 * @file ejercicio6.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 17-03-2017
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * @brief Funcion main del ejercicio6
 *
 * El programa consiste en la creacion de un proceso hijo, en el que en un
 * bucle continuo se van imprimiendo un mensaje por pantalla cada 5 segundos;
 * y en el padre, tras 30 segundos, se manda una señal de terminacion al hijo.
 *
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al usar el fork o el kill.
 */
int main() {
   pid_t pid;
   int kid;

   /* Creacion del proceso hijo */
   if ((pid = fork()) == -1) {
      /* Comprobacion de errores */
      printf("Error al ejecutar fork.\n");
      exit(EXIT_FAILURE);
   } else if (pid == 0) {
      /* Bucle infinito de escritura cada 5 segundos */
      while(1) {
         printf("Soy el proceso hijo con PID: %d\n", getpid());
         sleep(5);
      }
   } else {
      /* El padre duerme 30 segundos y manda la señal de terminacion al hijo */
      sleep(30);
      kid = kill(pid, 15);
      if (kid == -1) {
         /* Comprobacion de errores */
         printf("Error al enviar señal.\n");
         exit(EXIT_FAILURE);
      }
   }

   /* El padre espera a la terminacion del hijo */
   wait(NULL);
   return EXIT_SUCCESS;
}
