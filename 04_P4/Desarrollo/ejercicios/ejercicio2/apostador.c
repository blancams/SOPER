/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * Este modulo consiste en la creacion de una libreria que recoge el 
 * comportamiento del proceso apostador.
 *
 * @file apostador.c
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

#include "apostador.h"
#include "../../recursos/senales.h"
#include "../../recursos/mensajes.h"

#define MAX_APUESTA 500          /*!< Numero maximo de cantidad apostada */

int apostador(int key, int n_apostadores, int n_caballos) {
   int signvalue, msqid;
   char nom_apostador[13];
   sigset_t sset;
   apostador_gestor ap_generada;

   srand(time(NULL));

   if(pause() != -1){
      printf("Apostador: Fallo en pause de monitor.\n");
      exit(ERROR);
   }

   /* Acceso a la cola de mensajes */
   if(crear_cm(&msqid, key) == -1) {
      printf("Apostador: Fallo en el acceso a la cola de mensajes.\n");
      exit(ERROR);
   }

   /* Creacion de la mascara de señales */
   if (crear_mascara(&sset, SIGTERM) == -1) {
      printf("Apostador: Error al crear la mascara de senales.\n");
      exit(ERROR);
   }

   while(1) {
      usleep(100000);

<<<<<<< HEAD
      /* Generacion de la apuesta */
      ap_generada.tipo = 2;
=======
      ap_generada.tipo = 1;
>>>>>>> 3c22c24e7a182323954daf21ce3d87e8721d0217
      sprintf(nom_apostador, "Apostador-%d", rand() % n_apostadores + 1);
      strcpy(ap_generada.nombre, nom_apostador);
      ap_generada.caballo = rand() % n_caballos;
      ap_generada.apuesta = ((double)rand() / (double)RAND_MAX) * MAX_APUESTA;

      /* Envio del mensaje a la cola */
      if (enviar_m(msqid, (void *) &ap_generada, sizeof(apostador_gestor) - sizeof(long)) == -1) {
         printf("Apostador: Error al enviar mensaje desde generador.\n");
         exit(ERROR);
      }

      /* Comprobacion de recepcion de SIGTERM */
      if (senal_bloqueada(SIGTERM, &signvalue) == -1) {
         printf("Apostador: Error al comprobar si se ha detectado la senal.\n");
         exit(ERROR);
      }

      if (signvalue) {
         break;
      }
   }

   exit(OK);
}
