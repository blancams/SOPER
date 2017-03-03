/**
 * @brief Sistemas Operativos: Practica 1, ejercicio 9
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del noveno ejercicio de la primera practica,
 * referente al uso de la funcion "pipe" para la creacion de tuberias a modo de comunicacion
 * entre procesos padres e hijos.
 *
 * @file ejercicio9.c
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

#define NUM_PROC 4     /*!< Numero de procesos hijos a crear */
#define MAX_CHAR 150   /*!< Maxima longitud de cadenas de caracteres*/


/**
 * @brief funcion main del ejercicio9
 *
 * Este programa genera un conjunto de procesos (NUM_PROC) hijos que realizan cuatro
 * operaciones distintas: suma, resta, multiplicacion y division, acorde con su orden
 * de creacion. La comunicion entre padre e hijos se realiza a traves de una tuberia
 * por medio de la cual el padre pasa dos operandos al proceso hijo e imprime el resultado
 * de la operacion pertinente, devuelta por el hijo a traves de la tuberia.
 *
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al ejecutar las instrucciones fork() o pipe().
 */
int main (void){
   pid_t pid;
   int i;
   int fd1[2], fd2[2];
   char buf[MAX_CHAR];
   int op1, op2;

   for(i=0; i<NUM_PROC; i++) {
      /* Creacion de dos tuberias y comprobacion de errores */
      if(pipe(fd1)==-1){
         printf("Error creando la tubería.\n");
         exit(EXIT_FAILURE);
      }
      if(pipe(fd2)==-1){
         printf("Error creando la tubería.\n");
         exit(EXIT_FAILURE);
      }
      /* Comprobacion de error en el fork() */
      if((pid=fork())<0) {
         printf("Error haciendo fork().\n");
         exit(EXIT_FAILURE);
      } else if (pid==0) {
         /* Codigo del proceso hijo */
         close(fd1[1]);
         close(fd2[0]);
         /* Lectura por la primera tuberia de los operandos */
         read(fd1[0], buf, sizeof(buf));
         op1 = atoi(strtok(buf, ","));
         op2 = atoi(strtok(NULL,","));
         if(i==0){
            sprintf(buf, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Suma: %d\n", getpid(), op1, op2, op1+op2);
         } else if(i==1){
            sprintf(buf, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Resta: %d\n", getpid(), op1, op2, op1-op2);
         } else if(i==2){
            sprintf(buf, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Multiplicación: %d\n", getpid(), op1, op2, op1*op2);
         } else{
            sprintf(buf, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. División: %d\n", getpid(), op1, op2, op1/op2);
         }
         /* Escritura en la segunda tuberia del mensaje final */
         write(fd2[1], buf, strlen(buf)+1);
         exit(EXIT_SUCCESS);
      } else{
         /* Codigo del proceso padre */
         close(fd1[0]);
         close(fd2[1]);
         /* Lectura inicial de los operandos por pantalla */
         if(i==0){
            printf("Introduce dos números: ");
            scanf("%d %d", &op1, &op2);
         }
         /* Escritura en la primera tuberia de los operandos */
         sprintf(buf, "%d,%d", op1, op2);
         write(fd1[1], buf, strlen(buf)+1);
         /* Lectura en la segunda tuberia del mensaje a imprimir */
         read(fd2[0], buf, sizeof(buf));
         printf("%s", buf);
         /* Espera a la finalizacion del proceso hijo generado */
         waitpid(pid, NULL, 0);
      }
   }
   exit(EXIT_SUCCESS);
}
