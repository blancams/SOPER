/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * Este modulo consiste en la creacion
 * de una libreria que simplifique el uso de los semaforos.
 *
 * @file semaforos.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/shm.h>

#include "semaforos.h"

#define FILEKEY "/bin/cat"

int Inicializar_Semaforo(int semid, unsigned short *array) {
	union semun arg;
	arg.array = array;
	if(semctl (semid, 0, SETALL, arg) == -1){
      return ERROR;
   }
   return OK;
}

int Borrar_Semaforo(int semid) {
   if(semctl(semid, 0, IPC_RMID) == -1){
      return ERROR;
   }
   return OK;
}

int Crear_Semaforo(int key, int size, int *semid) {
	key_t k = ftok(FILEKEY, key);

   int id = semget(k, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);

   if(id == -1 && errno == EEXIST) {
      id = semget(key, size, SHM_R | SHM_W);
	}
	if(id == -1) {
      return ERROR;
   }

   *semid = id;
   return OK;
}

int Down_Semaforo(int id, int num_sem, short undo) {
   struct sembuf down;
   down.sem_num = num_sem;
   down.sem_op = -1;
   if(undo){
      down.sem_flg = SEM_UNDO;
   } else {
		down.sem_flg = 0;
	}
   if(semop(id, &down, 1) == -1){
      return ERROR;
   }
   return OK;
}

int DownMultiple_Semaforo(int id, int size, short undo, int *active) {
   int i;
   for(i = 0; i < size; i++) {
      if(Down_Semaforo(id, active[i], undo) == ERROR) {
         return ERROR;
      }
   }
   return OK;
}

int Up_Semaforo(int id, int num_sem, short undo) {
   struct sembuf up;
   up.sem_num = num_sem;
   up.sem_op = 1;
   if(undo) {
      up.sem_flg = SEM_UNDO;
   } else {
		up.sem_flg = 0;
	}
   if(semop(id, &up, 1) == -1) {
      return ERROR;
   }
   return OK;
}

int UpMultiple_Semaforo(int id,int size, short undo, int *active) {
   int i;
   for(i = 0; i < size; i++) {
      if(Up_Semaforo(id, active[i], undo) == ERROR) {
         return ERROR;
      }
   }
   return OK;
}
