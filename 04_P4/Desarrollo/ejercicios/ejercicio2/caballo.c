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

/**
 * @brief Funcion que recoge el comportamiento de cada uno de los procesos caballo.
 *
 * @param int i: Identificador del caballo.
 * @param int *fd: Descriptores de fichero para la comunicacion del proceso principal y los caballos.
 * @param int *posiciones: Array de las posiciones de todos los caballos.
 * @param int n_caballos: Numero de caballos.
 * @return int: -1 si ha ocurrido algun error, 0 en caso de ejecucion normal.
 */
int caballo(int i, int fd, int *posiciones, int n_caballos, int key) {
   int max, min, j, tirada, msqid, signvalue;
   sigset_t sset;
   caballo_principal mensaje;

   srand(time(NULL));

   if(crear_cm(&msqid, key)== -1){
      printf("Fallo en la creacion de la cola de mensajes (caballos).\n");
      exit(ERROR);
   }

   if (crear_mascara(&sset, SIGTERM) == -1) {
      printf("Apostador: Error al crear la mascara de senales.\n");
      exit(ERROR);
   }

   while(1) {
      if(pause() != -1){
         printf("No se en que momento da este error.\n");
         exit(ERROR);
      }

      if (senal_bloqueada(SIGTERM, &signvalue) == -1) {
         printf("Apostador: Error al comprobar si se ha detectado la senal.\n");
         exit(ERROR);
      }

      if (signvalue) {
         break;
      }

      read(fd, posiciones, sizeof(posiciones));
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

      if(enviar_m(msqid, &mensaje) == -1){
         printf("Error al mandar el mensaje desde los caballos al proceso principal.\n");
         exit(ERROR);
      }
   }

   return 0;

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
