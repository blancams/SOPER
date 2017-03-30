/**
 * @brief Sistemas Operativos: Practica 1, ejercicio 4b
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del cuarto (a) ejercicio de la primera practica,
 * referente al uso de del fork(), wait() y a los procesos huerfanos.
 *
 * @file ejercicio4b.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 01-03-2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define NUM_PROC 3   /*!< Numero de iteraciones del bucle que genera procesos */
#define MAX_CHAR 128 /*!< Maximo de caracteres para strings */

/**
 * @brief funcion main del ejercicio4a
 *
 * El programa va generando procesos en un bucle (por cada proceso existente crea un hijo) y,
 * en esta versión, se realiza un solo wait por cada proceso al final. Ademas, a traves del codigo
 * contenido al final del ejercicio mostramos el arbol del proceso raiz (el padre del primer proceso)
 * para observar si alguno de los subprocesos ha quedado huerfano.
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al ejecutar la funcion fork() o la funcion execvp.
 */
int main (void){
   pid_t pid;
   int i, ret;
   char pid_c[MAX_CHAR];

   /* Guardamos el PID del proceso raiz */
   sprintf(pid_c, "%d", getppid());

   /* Bucle que genera los procesos */
   for(i=0 ; i<NUM_PROC; i++) {
      if((pid=fork())<0) {
         printf("Error haciendo fork().\n");
         exit(EXIT_FAILURE);
      } else if (pid==0) {
         printf("HIJO %d, PID: %d, PPID: %d\n", i, getpid(), getppid());
      } else{
         printf("PADRE %d, PID: %d, PPID: %d\n", i, getpid(), getppid());
      }
   }

   /* Si wait() da error suele ser por usarlo en un proceso que no tiene hijos,
   en tal caso lo imprimimos por pantalla para saber que ha pasado por aqui;
   si no, se imprime el ID del proceso hijo que ha terminado */
   if ((ret = wait(NULL)) == -1) {
      if(errno == ECHILD) {
         printf("PID %d no tiene hijos.\n", getpid());
      }
   } else {
      printf("Terminado proceso %d.\n", ret);
   }

   /* Creacion de un nuevo hijo para la ejecucion del pstree del proceso raiz. Se deja
   un trazo del proceso, del padre y de la raiz, y el padre espera a que exactamente
   el hijo que ejecuta pstree termine con waitpid, dejando una traza de que, efectivamente,
   ha terminado. */
   char *buf[] = {"pstree", "-p", pid_c, NULL};
   if((pid=fork())<0) {
      exit(EXIT_FAILURE);
   } else if (pid==0) {
      printf("AUXPROC, PID: %d, PPID: %d, GPID: %s\n", getpid(), getppid(), pid_c);
      if (!execvp("pstree", buf)) {
         printf("Fallo al ejecutar funcion exe.\n");
         exit(EXIT_FAILURE);
      }
   } else{
      waitpid(pid, NULL, 0);
      printf("Terminado pstree para %d.\n", getpid());
   }

   exit(EXIT_SUCCESS);
}
