/**
 * @brief Sistemas Operativos: Practica 1, ejercicio 8
 *
 * En este modulo se ha implementado el codigo del octavo ejercicio de la primera practica,
 * referente al uso de las funciones "exe" para ejecutar comandos en terminal a traves de
 * procesos hijo.
 *
 * @file ejercicio8.c
 * @author Blanca Martín
 * @author Fernando Villar
 * @date 01-03-2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define MAX_CHAR 64 /*!< Maximo de caracteres para strings */
#define LENGTH_INS 2 /*!< 1 (null) + longitud en palabras de instrucciones para terminal */

/**
 * @brief funcion main del ejercicio8
 *
 * El programa espera comandos singulares como argumentos, hace comprobacion de los mismos,
 * y dependiendo del argumento final -l, -lp, -v o -vp, genera un proceso hijo con la funcion
 * exe correspondiente.
 * @param argc, numero de argumentos
 * @param argv, array de strings con los argumentos
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al reservar memoria, al introducir los argumentos de entrada
 * o al ejecutar la funcion fork().
 */
int main (int argc, char** argv) {
   int i;
   pid_t pid;
   char **argp;
   char path[MAX_CHAR];

   /* Comprobacion de los parametros de entrada */
   if (argc < 2) {
      fprintf(stdout, "No hay argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   } else if (strcmp(argv[argc-1], "-l")*strcmp(argv[argc-1], "-lp")*strcmp(argv[argc-1], "-v")*strcmp(argv[argc-1], "-vp")) {
      fprintf(stdout, "Error al introducir el parámetro para la función exe (-l, -lp, -v, -vp).\n");
      exit(EXIT_FAILURE);
   }

   /* Reserva de memoria para el array que va a contener los comandos */
   /* Se crean dos arrays de strings para introducir en la primera posicion
   el comando y en la segunda NULL */
   argp = malloc(LENGTH_INS * sizeof(char*));
   if (argp == NULL){
      exit(EXIT_FAILURE);
   }

   for (i = 0; i < LENGTH_INS; i++) {
      argp[i] = (char*) malloc(MAX_CHAR);
      if (argp[i] == NULL){
         exit(EXIT_FAILURE);
      }
   }

   for (i = 1; i < (argc-1); i++) {
      printf("\n");
      if ((pid = fork()) < 0) {
         /* Comprobacion de errores de fork() */
         fprintf(stdout, "Error haciendo fork().\n");
         exit(EXIT_FAILURE);
      } else if (pid == 0) {
         /* En el hijo, se comprueba el parametro y se ejecuta la funcion exe correspondiente */
         if (!strcmp(argv[argc-1], "-l")) {
            sprintf(path, "/bin/%s", argv[i]);
            if (execl(path, argv[i], NULL)) {
               sprintf(path, "/usr/bin/%s", argv[i]);
               execl(path, argv[i], NULL);
            }
         } else if (!strcmp(argv[argc-1], "-lp")) {
            execlp(argv[i], argv[i], NULL);
         } else if (!strcmp(argv[argc-1], "-v")) {
            strcpy(argp[0], argv[i]);
            argp[1] = NULL;
            sprintf(path, "/bin/%s", argv[i]);
            if (execl(path, argv[i], NULL)) {
               sprintf(path, "/usr/bin/%s", argv[i]);
               execl(path, argv[i], NULL);
            }
            execv(path, argp);
         } else {
            strcpy(argp[0], argv[i]);
            argp[1] = NULL;
            execvp(argv[i], argp);
         }

         /* Si el codigo llega aqui es que la funcion exe no se ha podido ejecutar
         y por tanto ha ocurrido lo que se imprime por pantalla */
         printf("Si este mensaje aparece se debe a que o bien se ha introducido ");
         printf("una instruccion incorrecta, o el ejecutable de dicha instruccion no ");
         printf("se encuentra ni en /usr/bin ni en /bin.\n");

      } else {
         /* El padre espera a que el hijo termine para cada fork() que ejecuta en el bucle */
         wait(NULL);
      }
   }

   /* Liberacion de memoria de los strings reservados al inicio del codigo */
   for (i = 0; i < LENGTH_INS; i++) {
      free(argp[i]);
   }
   free(argp);

   exit(EXIT_SUCCESS);
}
