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

#define BUF_SIZE 26
#define FILEKEY "/bin/cat"
#define KEY 1301
#define N_SEMAFOROS 3
#define SEMKEY 75846

struct buff {
   char buffer[BUF_SIZE];
   unsigned short n_char;
   unsigned short limite;
};

int productor(int semid, struct buff *buffer);
int consumidor(int semid, struct buff *buffer);

int main(void) {
   int i, pid, semid, shmid, key;
   struct buff *buffer;
   union semun arg;

   arg.array = (unsigned short*) malloc(N_SEMAFOROS*sizeof(unsigned short));

   if (arg.array == NULL) {
      printf("Error al reservar memoria.\n");
      exit(EXIT_FAILURE);
   }

   arg.array[0] = 1;
   arg.array[1] = 0;
   arg.array[2] = BUF_SIZE;

   if (Crear_Semaforo(SEMKEY, N_SEMAFOROS, &semid) == -1) {
      printf("Error al crear los semáforos.\n");
      exit(EXIT_FAILURE);
   }

   if (Inicializar_Semaforo(semid, arg.array) == -1) {
      printf("Error al inicializar los semáforos.\n");
      exit(EXIT_FAILURE);
   }

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

   for (i = 0; i < 2; i++) {
      if((pid = fork()) == -1) {
         printf("Error al hacer el fork.\n");
         exit(EXIT_FAILURE);
      } else if(!pid) {
         break;
      }
   }

   if((buffer = (struct buff *) shmat(shmid, NULL, 0)) == NULL){
      printf("Error en el acceso a la memoria compartida.\n");
      shmdt((char*) buffer);
      exit(EXIT_FAILURE);
   }

   if(pid) {
      buffer->n_char = 0;
      buffer->limite = 1;
      sleep(3);
      buffer->limite = 0;

      for (i = 0; i < 2; i++) {
         wait(NULL);
      }

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
      if (Down_Semaforo(semid, 2, 1) == -1) {
         printf("Error al ejecutar función Down_Semaforo.\n");
         return ERROR;
      }

      if (Down_Semaforo(semid, 0, 1) == -1) {
         printf("Error al ejecutar función Down_Semaforo.\n");
         return ERROR;
      }

      buffer->buffer[buffer->n_char] = 97 + buffer->n_char;
      buffer->n_char++;

      if (Up_Semaforo(semid, 1, 1) == -1) {
         printf("Error al ejecutar función Up_Semaforo.\n");
         return ERROR;
      }

      if (Up_Semaforo(semid, 0, 1) == -1) {
         printf("Error al ejecutar función Up_Semaforo.\n");
         return ERROR;
      }

      usleep(1000);
   }

   if (shmdt((char *) buffer) == -1) {
      printf("Error al dejar la memoria compartida.\n");
      exit(EXIT_FAILURE);
   }
   return OK;
}

int consumidor(int semid, struct buff *buffer) {
   while(buffer->limite) {
      if (Down_Semaforo(semid, 1, 1) == -1) {
         printf("Error al ejecutar función Down_Semaforo.\n");
         return ERROR;
      }

      if (Down_Semaforo(semid, 0, 1) == -1) {
         printf("Error al ejecutar función Down_Semaforo.\n");
         return ERROR;
      }

      buffer->n_char--;
      printf("Carácter leído: %c\n", buffer->buffer[buffer->n_char]);

      if (Up_Semaforo(semid, 2, 1) == -1) {
         printf("Error al ejecutar función Up_Semaforo.\n");
         return ERROR;
      }

      if (Up_Semaforo(semid, 0, 1) == -1) {
         printf("Error al ejecutar función Up_Semaforo.\n");
         return ERROR;
      }

      usleep(1000);
   }

   if (shmdt((char *) buffer) == -1) {
      printf("Error al dejar la memoria compartida.\n");
      exit(EXIT_FAILURE);
   }
   return OK;
}
