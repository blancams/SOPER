/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * Este modulo consiste en la creacion de una libreria que facilita
 * el trabajo con hilos.
 *
 * @file hilos.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
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

int salir_hilo(pthread_t hilo) {
   int ret;

   ret = pthread_cancel(hilo);

   if (ret == 0) {
      return OK;
   } else {
      return ERROR;
   }
}
