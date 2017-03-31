#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/shm.h>

#include "semaforos.h"

union semun {
	int val;
	struct semid_ds *semstat;
	unsigned short *array;
};

int Inicializar_Semaforo(int semid, unsigned short *array) {
	union semun arg;
	arg.array = array;
	if(semctl (semid, sizeof(array)/sizeof(array[0]), SETALL, arg) == -1){
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

int Crear_Semaforo(key_t key, int size, int *semid) {
   int id = semget(key, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);

   if(id == -1 && errno == EEXIST) {
      id = semget(key, size, SHM_R | SHM_W);
	}
	if(id == -1) {
      return ERROR;
   }

   *semid = id;
   return OK;
}

int Down_Semaforo(int id, int num_sem, int undo) {
   struct sembuf down;
   down.sem_num = num_sem;
   down.sem_op = -1;
   if(undo){
      down.sem_flg = SEM_UNDO;
   }
   if(semop(id, &down, 1) == -1){
      return ERROR;
   }
   return OK;
}

int DownMultiple_Semaforo(int id, int size, int undo, int *active) {
   int i;
   for(i = 0; i < size; i++) {
      if(Down_Semaforo(id, active[i], undo) == ERROR) {
         return ERROR;
      }
   }
   return OK;
}

int Up_Semaforo(int id, int num_sem, int undo) {
   struct sembuf up;
   up.sem_num = num_sem;
   up.sem_op = 1;
   if(undo) {
      up.sem_flg = SEM_UNDO;
   }
   if(semop(id, &up, 1) == -1) {
      return ERROR;
   }
   return OK;
}

int UpMultiple_Semaforo(int id,int size, int undo, int *active) {
   int i;
   for(i = 0; i < size; i++) {
      if(Up_Semaforo(id, active[i], undo) == ERROR) {
         return ERROR;
      }
   }
   return OK;
}
