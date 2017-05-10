#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "memcomp.h"

#define OK 2
#define ERROR -1
#define FILEKEY "/bin/cat"    /*!< Fichero para la generacion de la clave */

int crear_shm(int size, int *shmid, int key) {
   key_t k;
   int id;

   k = ftok(FILEKEY, key);
   if (k == (key_t) -1) {
      return ERROR;
   }

   id = shmget(k, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
   if (id == -1) {
      return ERROR;
   } else {
      *shmid = id;
      return OK;
   }
}

int acceder_shm(int shmid, char* addr) {
   char *shmaddr;

   shmaddr = shmat(shmid, NULL, 0);

   if (shmaddr == NULL) {
      return ERROR;
   } else {
      addr = shmaddr;
      return OK;
   }
}

int salir_shm(char* addr) {
   int ret;

   ret = shmdt(addr);

   if (ret == 0) {
      return OK;
   } else {
      return ERROR;
   }
}

int eliminar_shm(int shmid) {
   shmctl(shmid, IPC_RMID, (struct shmid_ds*) NULL);

   return OK;
}
