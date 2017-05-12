/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * Este modulo consiste en la creacion de una libreria que recoge el
 * comportamiento del monitor de la carrera.
 *
 * @file monitor.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <limits.h>

#include "../../recursos/memcomp.h"
#include "../../recursos/senales.h"
#include "../../recursos/semaforos.h"
#include "gestor.h"
#include "monitor.h"

#define MAX_CHAR 512             /*!< Numero maximo de caracteres */

int monitor(int *shmid_apuestas, int shmid_posiciones, int n_caballos, int n_apostadores, int semid) {
   int j, signvalue;
   int *posiciones;
   char estado[MAX_CHAR];
   sigset_t sset;
   apuestas_total *apuestas;

   /* Acceso a las regiones de memoria compartida de las apuestas */
   if ((apuestas = (apuestas_total *) acceder_shm(shmid_apuestas[0])) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) apuestas);
      libera_recursos_monitor(NULL, NULL, n_apostadores);
      return ERROR;
   }
   if ((apuestas->apostado = (double *) acceder_shm(shmid_apuestas[1])) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) apuestas->apostado);
      libera_recursos_monitor(NULL, NULL, n_apostadores);
      return ERROR;
   }
   if ((apuestas->ganancia = (double **) acceder_shm(shmid_apuestas[2])) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) apuestas->ganancia);
      libera_recursos_monitor(NULL, NULL, n_apostadores);
      return ERROR;
   }
   if ((apuestas->cotizacion = (double *) acceder_shm(shmid_apuestas[3])) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) apuestas->cotizacion);
      libera_recursos_monitor(NULL, NULL, n_apostadores);
      return ERROR;
   }
   for (j = 4; j < n_apostadores + 4; j++) {
      if ((apuestas->ganancia[j-4] = (double *) acceder_shm(shmid_apuestas[j])) == (void *) -1) {
         printf("Error al acceder a memoria compartida en gestor.\n");
         salir_shm((void *) apuestas->ganancia[j-4]);
         libera_recursos_monitor(NULL, NULL, n_apostadores);
         return ERROR;
      }
   }
   /* Acceso a la memoria compartida de las posiciones de los caballos */
   if ((posiciones = (int *) acceder_shm(shmid_posiciones)) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) posiciones);
      libera_recursos_monitor(apuestas, NULL, n_apostadores);
      return ERROR;
   }

   /* Creacion de la mascara de señales */
   if (crear_mascara(&sset, SIGTERM) == -1) {
      printf("Apostador: Error al crear la mascara de senales.\n");
      libera_recursos_monitor(apuestas, posiciones, n_apostadores);
      return ERROR;
   }

   /* Espera a la señalizacion del proceso gestor */
   if(pause() != -1){
      printf("Fallo en pause de monitor 1.\n");
      libera_recursos_monitor(apuestas, posiciones, n_apostadores);
      return ERROR;
   }
   /* Impresion de los datos antes de comenzar la carrera */
   for (j = 0; j < 15; j++) {
      sprintf(estado, "Estado de la carrera: faltan %d segundos.", 15-j);

      if (Down_Semaforo(semid, 0, 0) == -1) {
         printf("Error al ejecutar función Down_Semaforo bla.\n");
         libera_recursos_monitor(apuestas, posiciones, n_apostadores);
         return ERROR;
      }

      imprimir_carrera(estado, n_caballos, posiciones, apuestas->cotizacion);

      if (Up_Semaforo(semid, 0, 0) == -1) {
         printf("Error al ejecutar función Up_Semaforo.\n");
         libera_recursos_monitor(apuestas, posiciones, n_apostadores);
         return ERROR;
      }

      usleep(1000000);
   }
   /* Impresion de los datos de la carrera comenzada */
   sprintf(estado, "Estado de la carrera: comenzada.");

   while(1) {
      /* Espera a la señalizacion */
      if(pause() != -1){
         printf("Fallo en pause de monitor 3.\n");
         libera_recursos_monitor(apuestas, posiciones, n_apostadores);
         return ERROR;
      }
      /* Comprobacion de recepcion de SIGTERM */
      if (senal_bloqueada(SIGTERM, &signvalue) == -1) {
         printf("Apostador: Error al comprobar si se ha detectado la senal.\n");
         libera_recursos_monitor(apuestas, posiciones, n_apostadores);
         return ERROR;
      }
      if (signvalue) {
         break;
      }

      imprimir_carrera(estado, n_caballos, posiciones, apuestas->cotizacion);
   }
   /* Impresion de la carrera finalizada */
   printf("#################################\n");
   printf("       CARRERA FINALIZADA        \n");
   printf("#################################\n");

   for (j = 0; j < 15; j++) {
      printf("Resultados en %d segundos...\n", 15-j);
      usleep(1000000);
   }

   imprimir_finalizada(n_caballos, n_apostadores, posiciones, apuestas->ganancia);
   /* Liberacion de recursos */
   libera_recursos_monitor(apuestas, posiciones, n_apostadores);

   return OK;
}

void imprimir_carrera(char *estado, int n_caballos, int *posiciones, double *cotizaciones) {
   int i;

   printf("#################################\n");
   printf("%s\n", estado);
   if (!strcmp("Estado de la carrera: comenzada.", estado)) {
      /* Si la carrera ya ha comenzado imprimimos posiciones de los caballos */
      for (i = 0; i < n_caballos; i++) {
         printf("Posición del caballo %d: %d\n", i+1, posiciones[i]);
      }
   } else {
      /* Si la carrera no ha comenzado imprimimos cotizaciones de los caballos */
      for (i = 0; i < n_caballos; i++) {
         printf("Cotización del caballo %d: %lf\n", i+1, cotizaciones[i]);
      }
   }
   printf("#################################\n");

}

 void imprimir_finalizada(int n_caballos, int n_apostadores, int *posiciones, double **ganancia) {
   int i, j;
   int max1, max2, max3, ind1, ind2, ind3, indap;
   double benef[n_apostadores], max;

   /* Calculo de las tres primeras posiciones */
   for (i = 0, max1 = 0, max2 = 0, max3 = 0; i < n_caballos; i++) {
      if (max3 < posiciones[i]) {
         max3 = posiciones[i];
         ind3 = i;
         if (max2 < posiciones[i]) {
            max3 = max2;
            ind3 = ind2;
            max2 = posiciones[i];
            ind2 = i;
            if (max1 < posiciones[i]) {
               max2 = max1;
               ind2 = ind1;
               max1 = posiciones[i];
               ind1 = i;
            }
         }
      }
   }
   /* Calculo de los maximos beneficios */
   for (i = 0, max = INT_MIN; i < n_apostadores; i++) {
      benef[i] = 0;
      for (j = 0; j < n_caballos; j++) {
         if (j == ind1) {
            benef[i] += ganancia[i][j];
         } else {
            benef[i] -= ganancia[i][j];
         }
      }

      if (max < benef[i]) {
         max = benef[i];
         indap = i;
      }
   }
   /* Impresion de los resultados de la carrera */
   printf("#################################\n");
   printf("Primer puesto: Caballo %d - %d\n", ind1+1, max1);
   printf("Segundo puesto: Caballo %d - %d\n", ind2+1, max2);
   printf("Tercer puesto: Caballo %d - %d\n", ind3+1, max3);
   printf("#################################\n");

   for (i = 0; i < n_apostadores; i++) {
      printf("Apostador-%d: ", i+1);
      fflush(NULL);
      if (benef[i] >= 0) {
         printf("+%lf", benef[i]);
         fflush(NULL);
      } else {
         printf("%lf", benef[i]);
         fflush(NULL);
      }
      if (i == indap) {
         printf(": GANADOR\n");
      } else {
         printf("\n");
      }
   }

   printf("#################################\n");
}

void libera_recursos_monitor(apuestas_total *apuestas, int *posiciones, int n_apostadores) {
   int i;
   /* Liberacion de las regiones de memoria compartida */
   if (apuestas != NULL) {
      if (salir_shm((void*) apuestas->apostado) == -1) {
         printf("Error al salir de memoria compartida en monitor.\n");
         return;
      }
      if (salir_shm((void*) apuestas->cotizacion) == -1) {
         printf("Error al salir de memoria compartida en monitor.\n");
         return;
      }
      for (i = 0; i < n_apostadores; i++) {
         if (salir_shm((void*) apuestas->ganancia[i]) == -1) {
            printf("Error al salir de memoria compartida en monitor.\n");
            return;
         }
      }
      if (salir_shm((void*) apuestas->ganancia) == -1) {
         printf("Error al salir de memoria compartida en monitor.\n");
         return;
      }
      if (salir_shm((void*) apuestas) == -1) {
         printf("Error al salir de memoria compartida en monitor.\n");
         return;
      }
   }
   if (posiciones != NULL) {
      if (salir_shm((void*) posiciones) == -1) {
         printf("Error al salir de memoria compartida en monitor.\n");
         return;
      }
   }
}
