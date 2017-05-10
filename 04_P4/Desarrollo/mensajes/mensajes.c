#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

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

/*
int enviar_m(int msid, char* mensaje, long tipo) {
   int ret;
   mensaje msg;

   if (strlen(mensaje) >= MAX_CHAR) {
      return ERROR;
   }

   msg.tipo = tipo;
   msg.mensaje = mensaje;

   ret = msgsnd(msid, &msg, sizeof(mensaje) - sizeof(long), 0);

   if (ret == 0) {
      return OK;
   } else {
      return ERROR;
   }
}
*/

int enviar_m(int msid, void *mensaje){
   if(msgsnd(msid, mensaje, sizeof(mensaje) - sizeof(long), 0) == -1){
      return ERROR;
   }
   return OK;
}

/*
int recibir_m(int msid, long tipo) {
   ssize_t ret;
   mensaje msg;

   ret = msgrcv(msid, &msg, sizeof(mensaje) - sizeof(long), tipo, 0);

   if (ret == -1) {
      return ERROR;
   } else {
      return OK;
   }
}
*/

int recibir_m(int msid, void *mensaje, long tipo){
   if(msgrcv(msid, mensaje, sizeof(mensaje) - sizeof(long), tipo, 0) == -1){
      return ERROR;
   }
   return OK;
}

int eliminar_cm(int msid) {
   msgctl(msid, IPC_RMID, (struct msqid_ds *) NULL);

   return OK;
}
