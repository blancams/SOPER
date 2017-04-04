/**
 * @brief Sistemas Operativos: Practica 3, ejercicio 6
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del sexto
 * ejercicio de la tercera practica, que consiste en la implementacion del
 * problema del productor-consumidor.
 *
 * @file ejercicio6.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 07-04-2017
 */
 #include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../semaforos.h"

#define BUF_SIZE 26           /*!< Maximo numero de caracteres */
#define FILEKEY "/bin/cat"    /*!< Fichero para la generacion de la clave */
#define KEY 1301              /*!< Numero para la generacion de la clave de la memoria compartida*/
#define N_SEMAFOROS 3         /*!< Numero de semaforos */
#define SEMKEY 75846          /*!< Numero para la generacion de la clave de los semaforos */

/**
 * @brief Estructura que se almacenara en la region de memoria compartida.
 *
 * Estructura que contiene una cadena de caracteres, y dos numeros que se 
 * almacenara en la region de memoria compartida para lectura y escritura de 
 * los procesos. 
 */
struct buff {
   char buffer[BUF_SIZE];
   unsigned short n_char;
   unsigned short limite;
};

/**
 * @brief Productor
 *
 * Funcion que accede, haciendo uso de semaforos, a la region de memoria
 * compartida y escribe en ella caracteres generados en orden alfabetico. 
 *
 * @param semid, identificador del semaforo.
 * @param buffer, estructura de tipo buff.
 * @return OK si todas las operaciones se realizaron con exito, ERROR si 
 * se ha producido algun fallo en la manipulacion de los semaforos.
 */
int productor(int semid, struct buff *buffer);

/**
 * @brief Consumidor.
 *
 * Funcion que accede, haciendo uso de semaforos, a la region de memoria
 * compartida y lee de ella los caracteres escritos por el productor y los imprime
 * por pantalla.
 *
 * @param semid, identificador del semaforo.
 * @param buffer, estructura de tipo buff.
 * @return OK si todas las operaciones se realizaron con exito, ERROR si 
 * se ha producido algun fallo en la manipulacion de los semaforos.
 */
int consumidor(int semid, struct buff *buffer);

/**
 * @brief Funcion main del ejercicio6
 *
 * El programa consiste en la creacion e inicializacion de una serie de semaforos
 * que regularan el acceso a una region de memoria compartida. A esta region accederan
 * dos procesos, uno que escribira en ella y otro que leera. 
 *
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al reservar memoria, al crear e inicializar los semaforos,
 * al ejecutar la funcion fork(), al establecer la region de memoria
 * compartida o al desvincularse de la misma.
 */
int main(void) {
   int i, pid, semid, shmid, key;
   struct buff *buffer;
   union semun arg;

   /* Reserva de memoria */
   arg.array = (unsigned short*) malloc(N_SEMAFOROS*sizeof(unsigned short));

   if (arg.array == NULL) {
      printf("Error al reservar memoria.\n");
      exit(EXIT_FAILURE);
   }

   arg.array[0] = 1;
   arg.array[1] = 0;
   arg.array[2] = BUF_SIZE;

   /* Ceacion e inicializacion de los semaforos */
   if (Crear_Semaforo(SEMKEY, N_SEMAFOROS, &semid) == -1) {
      printf("Error al crear los semáforos.\n");
      exit(EXIT_FAILURE);
   }

   if (Inicializar_Semaforo(semid, arg.array) == -1) {
      printf("Error al inicializar los semáforos.\n");
      exit(EXIT_FAILURE);
   }

   /* Creacion de la region de memoria compartida */
   key = ftok(FILEKEY, KEY);
   if(key == -1){
      printf("Error al generar la clave.\n");
      exit(EXIT_FAILURE);
   }

   shmid = shmget(key, sizeof(struct buff), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
   if(shmid == -1){
      printf("Error en la reserva del bloque de memoria.\n");
      exit(EXIT_FAILURE);
   }

   /* Creacion de los procesos consumidor y productor */
   for (i = 0; i < 2; i++) {
      if((pid = fork()) == -1) {
         printf("Error al hacer el fork.\n");
         exit(EXIT_FAILURE);
      } else if(!pid) {
         break;
      }
   }

   /* Vinculacion a la region de memoria compartida */
   if((buffer = (struct buff *) shmat(shmid, NULL, 0)) == NULL){
      printf("Error en el acceso a la memoria compartida.\n");
      shmdt((char*) buffer);
      exit(EXIT_FAILURE);
   }

   if(pid) {
      /* Codigo del padre */
      buffer->n_char = 0;
      buffer->limite = 1;
      sleep(3);
      buffer->limite = 0;

      /* Espera a la finalizacion de los procesos hijos */
      for (i = 0; i < 2; i++) {
         wait(NULL);
      }

      /* Liberacion de recursos */
      if (Borrar_Semaforo(semid) == -1) {
         printf("Error al borrar los semáforos.\n");
         exit(EXIT_FAILURE);
      }
      if (shmdt((char *) buffer) == -1) {
         printf("Error al dejar la memoria compartida.\n");
         exit(EXIT_FAILURE);
      }
      shmctl(shmid, IPC_RMID, (struct shmid_ds *) NULL);
      free(arg.array);
   } else if (i == 0) {
      /* Codigo del productor */
      sleep(1);
      if (productor(semid, buffer) == ERROR) {
         printf("El error se ha producido en productor.\n");
         if (shmdt((char *) buffer) == -1) {
            printf("Error al dejar la memoria compartida.\n");
         }
         exit(EXIT_FAILURE);
      }
      free(arg.array);
   } else {
      /* Codigo del consumidor */
      sleep(1);
      if (consumidor(semid, buffer) == ERROR) {
         printf("El error se ha producido en consumidor.\n");
         if (shmdt((char *) buffer) == -1) {
            printf("Error al dejar la memoria compartida.\n");
         }
         exit(EXIT_FAILURE);
      }
      free(arg.array);
   }

   exit(EXIT_SUCCESS);
}

int productor(int semid, struct buff *buffer) {
   while (buffer->limite) {
<<<<<<< HEAD
      /* Acceso controlado a la region de memoria compartida */
      if (Down_Semaforo(semid, 2, 1) == -1) {
=======
      if (Down_Semaforo(semid, 2, 0) == -1) {
>>>>>>> ebb472873fef906ba3a9ee9753e121c81ca779ba
         printf("Error al ejecutar función Down_Semaforo.\n");
         return ERROR;
      }

      if (Down_Semaforo(semid, 0, 0) == -1) {
         printf("Error al ejecutar función Down_Semaforo.\n");
         return ERROR;
      }

      /* Escritura */
      buffer->buffer[buffer->n_char] = 97 + buffer->n_char;
      buffer->n_char++;

<<<<<<< HEAD
      /* Fin del uso de la memoria compartida */
      if (Up_Semaforo(semid, 1, 1) == -1) {
=======
      if (Up_Semaforo(semid, 1, 0) == -1) {
>>>>>>> ebb472873fef906ba3a9ee9753e121c81ca779ba
         printf("Error al ejecutar función Up_Semaforo.\n");
         return ERROR;
      }

      if (Up_Semaforo(semid, 0, 0) == -1) {
         printf("Error al ejecutar función Up_Semaforo.\n");
         return ERROR;
      }

      usleep(1000);
   }

   /* Desvinculacion de la region de memoria compartida */
   if (shmdt((char *) buffer) == -1) {
      printf("Error al dejar la memoria compartida.\n");
      exit(EXIT_FAILURE);
   }
   return OK;
}

int consumidor(int semid, struct buff *buffer) {
   while(buffer->limite) {
<<<<<<< HEAD
      /* Acceso controlado a la region de memoria compartida */
      if (Down_Semaforo(semid, 1, 1) == -1) {
=======
      if (Down_Semaforo(semid, 1, 0) == -1) {
>>>>>>> ebb472873fef906ba3a9ee9753e121c81ca779ba
         printf("Error al ejecutar función Down_Semaforo.\n");
         return ERROR;
      }

      if (Down_Semaforo(semid, 0, 0) == -1) {
         printf("Error al ejecutar función Down_Semaforo.\n");
         return ERROR;
      }

      /* Lectura */
      buffer->n_char--;
      printf("Carácter leído: %c\n", buffer->buffer[buffer->n_char]);

<<<<<<< HEAD
      /* Fin del uso de la memoria compartida */
      if (Up_Semaforo(semid, 2, 1) == -1) {
=======
      if (Up_Semaforo(semid, 2, 0) == -1) {
>>>>>>> ebb472873fef906ba3a9ee9753e121c81ca779ba
         printf("Error al ejecutar función Up_Semaforo.\n");
         return ERROR;
      }

      if (Up_Semaforo(semid, 0, 0) == -1) {
         printf("Error al ejecutar función Up_Semaforo.\n");
         return ERROR;
      }

      usleep(1000);
   }

   /* Desvinculacion de la region de memoria compartida */
   if (shmdt((char *) buffer) == -1) {
      printf("Error al dejar la memoria compartida.\n");
      exit(EXIT_FAILURE);
   }
   return OK;
}
