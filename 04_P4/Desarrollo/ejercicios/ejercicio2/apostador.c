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

#define MAX_APUESTA 20          /*!< Numero maximo de cantidad apostada */

int apostador(int key, int n_apostadores, int n_caballos) {
   int signvalue, msqid, num_ap;
   sigset_t sset;
   apostador_gestor *ap_generada;
   /* Semilla */
   srand(time(NULL));
   /* Espera a la señalizacion del proceso principal */
   if(pause() != -1){
      printf("Apostador: Fallo en pause de apostador.\n");
      return ERROR;
   }
   /* Acceso a la cola de mensajes */
   if(crear_cm(&msqid, key) == -1) {
      printf("Apostador: Fallo en el acceso a la cola de mensajes.\n");
      return ERROR;
   }
   /* Creacion de la mascara de señales */
   if (crear_mascara(&sset, SIGTERM) == -1) {
      printf("Apostador: Error al crear la mascara de senales.\n");
      return ERROR;
   }

   while(1) {
      usleep(100000);
      /* Generacion de la apuesta */
      ap_generada = malloc (sizeof(apostador_gestor));
      ap_generada->tipo = 1;
      num_ap = rand()%n_apostadores + 1;
      sprintf(ap_generada->ap.nombre, "Apostador-%d", num_ap);
      while (strlen(ap_generada->ap.nombre) != NOM_CHAR-1) {
         strcat(ap_generada->ap.nombre, "1");
      }
      ap_generada->ap.caballo = rand() % n_caballos;
      ap_generada->ap.apuesta = ((double)rand() / (double)RAND_MAX) * (double)MAX_APUESTA;

      /* Envio del mensaje a la cola */
      if (enviar_m(msqid, ap_generada, sizeof(info_ag)) == -1) {
         printf("Apostador: Error al enviar mensaje desde generador.\n");
         free(ap_generada);
         return ERROR;
      }
      /* Liberacion de recursos */
      free(ap_generada);

      /* Comprobacion de recepcion de SIGTERM */
      if (senal_bloqueada(SIGTERM, &signvalue) == -1) {
         printf("Apostador: Error al comprobar si se ha detectado la senal.\n");
         return ERROR;
      }
      if (signvalue) {
         break;
      }
   }

   return OK;
}
