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

#include "../../recursos/memcomp.h"
#include "../../recursos/senales.h"
#include "../../recursos/semaforos.h"
#include "gestor.h"
#include "monitor.h"

#define MAX_CHAR 512             /*!< Numero maximo de caracteres */

int monitor(int *shmid_apuestas, int shmid_posiciones, int n_caballos, int semid) {
   int j, signvalue;
   int *posiciones;
   char estado[MAX_CHAR];
   sigset_t sset;
   apuestas_total *apuestas;

   /* Acceso a las regiones de memoria compartida */
   if ((apuestas = (apuestas_total *) acceder_shm(shmid_apuestas[0])) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) apuestas);
      libera_recursos_monitor(NULL, NULL);
      exit(ERROR);
   }

   if ((apuestas->apostado = (double *) acceder_shm(shmid_apuestas[1])) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) apuestas->apostado);
      libera_recursos_monitor(NULL, NULL);
      exit(ERROR);
   }

   if ((apuestas->ganancia = (double *) acceder_shm(shmid_apuestas[2])) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) apuestas->ganancia);
      libera_recursos_monitor(NULL, NULL);
      exit(ERROR);
   }

   if ((apuestas->cotizacion = (double *) acceder_shm(shmid_apuestas[3])) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) apuestas->cotizacion);
      libera_recursos_monitor(NULL, NULL);
      exit(ERROR);
   }

   if ((posiciones = (int *) acceder_shm(shmid_posiciones)) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) posiciones);
      libera_recursos_monitor(apuestas, NULL);
      exit(ERROR);
   }

   /* Creacion de la mascara de señales */
   if (crear_mascara(&sset, SIGTERM) == -1) {
      printf("Apostador: Error al crear la mascara de senales.\n");
      libera_recursos_monitor(apuestas, posiciones);
      exit(ERROR);
   }

   /* Espera a la señalizacion del proceso gestor */
   if(pause() != -1){
      printf("Fallo en pause de monitor 1.\n");
      libera_recursos_monitor(apuestas, posiciones);
      exit(ERROR);
   }

   for (j = 0; j < 5; j++) {
      sprintf(estado, "Estado de la carrera: faltan %d segundos.", 5-j);

      if (Down_Semaforo(semid, 0, 0) == -1) {
         printf("Error al ejecutar función Down_Semaforo.\n");
         libera_recursos_monitor(apuestas, posiciones);
         exit(ERROR);
      }

      imprimir_carrera(estado, n_caballos, posiciones, apuestas->cotizacion);

      if (Up_Semaforo(semid, 0, 0) == -1) {
         printf("Error al ejecutar función Up_Semaforo.\n");
         libera_recursos_monitor(apuestas, posiciones);
         exit(ERROR);
      }

      usleep(1000000);
   }

   /* Impresion de los datos de la carrera comenzada */
   sprintf(estado, "Estado de la carrera: comenzada.");

   while(1) {
      if(pause() != -1){
         printf("Fallo en pause de monitor 3.\n");
         libera_recursos_monitor(apuestas, posiciones);
         exit(ERROR);
      }
      
      if (senal_bloqueada(SIGTERM, &signvalue) == -1) {
         printf("Apostador: Error al comprobar si se ha detectado la senal.\n");
         libera_recursos_monitor(apuestas, posiciones);
         exit(ERROR);
      }

      if (signvalue) {
         break;
      }

      imprimir_carrera(estado, n_caballos, posiciones, apuestas->cotizacion);
   }

   /* Espera de 15 segundos */
   if (alarm(15) == -1) {
      printf("Fallo al crear alarma.\n");
      libera_recursos_monitor(apuestas, posiciones);
      exit(ERROR);
   }

   if (pause() != -1) {
      printf("Fallo en pause de monitor.\n");
      libera_recursos_monitor(apuestas, posiciones);
      exit(ERROR);
   }

   imprimir_finalizada(n_caballos, posiciones, apuestas->ganancia);

   libera_recursos_monitor(apuestas, posiciones);

   exit(OK);
}

/**
 * @brief Funcion que imprime la informacion de la carrera.
 *
 * @param char *estado: Mensaje con el estado de la carrera.
 * @param int n_caballos: Numero de caballos.
 * @param int *posiciones: Posiciones de los caballos.
 * @param double *cotizaciones: Cotizaciones de cada caballo.
 */
void imprimir_carrera(char *estado, int n_caballos, int *posiciones, double *cotizaciones) {
   int i;

   printf("#################################\n");
   printf("%s\n", estado);
   if (!strcmp("Estado de la carrera: comenzada.", estado)) {
      for (i = 0; i < n_caballos; i++) {
         printf("Posición del caballo %d: %d\n", i+1, posiciones[i]);
      }
   } else {
      for (i = 0; i < n_caballos; i++) {
         printf("Cotización del caballo %d: %lf\n", i+1, cotizaciones[i]);
      }
   }
   printf("#################################\n");

}

/**
 * @brief Funcion que imprime la informacion de la carrera una vez finalizada.
 *
 * @param int n_caballos: Numero de caballos.
 * @param int *posiciones: Posiciones de los caballos.
 * @param double *ganancia: Ganancia de cada uno de los apostadores.
 */
 void imprimir_finalizada(int n_caballos, int *posiciones, double *ganancia) {
   int i, max1, max2, max3, ind1, ind2, ind3;

   printf("#################################\n");
   printf("Carrera finalizada.\n");
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
   printf("Primer caballo: %d - %d\n", ind1+1, max1);
   printf("Segundo caballo: %d - %d\n", ind2+1, max2);
   printf("Tercer caballo: %d - %d\n", ind3+1, max3);
}

void libera_recursos_monitor(apuestas_total *apuestas, int *posiciones) {
   if (apuestas != NULL) {
      if (salir_shm((char*) apuestas) == -1) {
         printf("Error al salir de memoria compartida en monitor.\n");
         exit(ERROR);
      }
   }

   if (posiciones != NULL) {
      if (salir_shm((char*) posiciones) == -1) {
         printf("Error al salir de memoria compartida en monitor.\n");
         exit(ERROR);
      }
   }
}
