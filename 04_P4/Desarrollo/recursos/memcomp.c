/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * Este modulo consiste en la creacion
 * de una libreria que simplifique el uso de la memoria compartida.
 *
 * @file memcomp.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "memcomp.h"

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
      id = shmget(k, size, SHM_R | SHM_W);
      if (id == -1) {
         return ERROR;
      } else {
         *shmid = id;
         return OK;
      }
   } else {
      *shmid = id;
      return OK;
   }
}

void* acceder_shm(int shmid) {
   return shmat(shmid, NULL, 0);
}

int salir_shm(void* addr) {
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
