/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * Este modulo consiste en la creacion de una libreria que recoge el 
 * comportamiento de los caballos.
 *
 * @file caballo.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <limits.h>

#include "../../recursos/mensajes.h"
#include "../../recursos/senales.h"
#include "caballo.h"


int caballo(int i, int fd, int n_caballos, int key) {
   int max, min, j, tirada, msqid, signvalue;
   int *posiciones = NULL;
   sigset_t sset;
   caballo_principal mensaje;

   srand(time(NULL));

   /* Acceso a la cola de mensajes */
   if(crear_cm(&msqid, key)== -1){
      printf("Fallo en la creacion de la cola de mensajes (caballos).\n");
      exit(ERROR);
   }

   /* Creacion de la mascara de señales */
   if (crear_mascara(&sset, SIGTERM) == -1) {
      printf("Apostador: Error al crear la mascara de senales.\n");
      exit(ERROR);
   }

   printf("Caballo llega aquí?\n");

   while(1) {
      /* Espera a la señalizacion del proceso principal */
      if(pause() != -1){
         printf("Error en el pause de los caballos.\n");
         exit(ERROR);
      }

      /* Comprobacion de la señal SIGTERM */
      if (senal_bloqueada(SIGTERM, &signvalue) == -1) {
         printf("Apostador: Error al comprobar si se ha detectado la senal.\n");
         exit(ERROR);
      }

      if (signvalue) {
         break;
      }

      /* Lectura de las posiciones */
      read(fd, posiciones, sizeof(int) * n_caballos);
      max = 0;
      min = INT_MAX;

      for(j = 0; j < n_caballos; j++){
         if(max < posiciones[j]){
            max = posiciones[j];
         }
         if(min > posiciones[j]){
            min = posiciones[j];
         }
      }

      /* Generacion de la tirada */
      if (max) {
         if(posiciones[i] == max){
            tirada = tirada_ganadora();
         } else if(posiciones[i] == min){
            tirada = tirada_remontadora();
         } else{
            tirada = tirada_normal();
         }
      } else {
         tirada = tirada_normal();
      }

      mensaje.tipo = 1;
      mensaje.tirada = tirada;

<<<<<<< HEAD
      /* Envio del mensaje con la tirada */
      if(enviar_m(msqid, &mensaje, sizeof(caballo_principal) - sizeof(long)) == -1){
=======
      if(enviar_m(msqid, (void *) &mensaje, sizeof(caballo_principal) - sizeof(long)) == -1){
>>>>>>> 3c22c24e7a182323954daf21ce3d87e8721d0217
         printf("Error al mandar el mensaje desde los caballos al proceso principal.\n");
         exit(ERROR);
      }
   }

   exit(0);

}

int tirada_normal(){
   return rand() % 6 + 1;
}

int tirada_ganadora(){
   return rand() % 7 + 1;
}

int tirada_remontadora(){
   return tirada_normal() + tirada_normal();
}
