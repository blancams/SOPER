#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "hilos.h"

int crear_hilo(pthread_t *hilo, void*(*funcion)(void*), void* args) {
   int ret;

   ret = pthread_create(hilo, NULL, funcion, args);

   if (ret == 0) {
      return OK;
   } else {
      return ERROR;
   }
}

int unir_hilo(pthread_t hilo) {
   int ret;

   ret = pthread_join(hilo, NULL);

   if (ret == 0) {
      return OK;
   } else {
      return ERROR;
   }
}

int salir_hilo() {
   pthread_exit(NULL);

   return OK;
}
