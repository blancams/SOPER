/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * Este modulo consiste en la creacion
 * de una libreria que simplifique el uso de mensajes.
 *
 * @file mensajes.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

#include "mensajes.h"

#define OK 2
#define ERROR -1
#define FILEKEY "/bin/cat"

int crear_cm(int *cmid, int key) {
   key_t clave;
   int msqid;

   clave = ftok(FILEKEY, key);
   if (clave == (key_t) -1) {
      return ERROR;
   }

   msqid = msgget(clave, IPC_CREAT | IPC_EXCL | 0660);
   if (msqid == -1) {
      msqid = msgget(clave, 0660);
      if (msqid == -1) {
         return ERROR;
      }
      *cmid = msqid;
      return OK;
   } else {
      *cmid = msqid;
      return OK;
   }
}

int enviar_m(int msid, void *mensaje, int size){
   if(msgsnd(msid, mensaje, size, 0) == -1){
      return ERROR;
   }
   return OK;
}

int recibir_m(int msid, void *mensaje, long tipo, int size){
   if(msgrcv(msid, mensaje, size, tipo, 0) == -1){
      //if (errno != EINTR {
         return ERROR;
      //} else {
      //   return OK;
      //}
   }
   return OK;
}

int eliminar_cm(int msid) {
   msgctl(msid, IPC_RMID, (struct msqid_ds *) NULL);

   return OK;
}
