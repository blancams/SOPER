#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "../../recursos/senales.h"
#include "../../recursos/hilos.h"
#include "../../recursos/mensajes.h"
#include "../../recursos/semaforos.h"
#include "../../recursos/memcomp.h"
#include "gestor.h"
#include "apostador.h"

int gestor(int shmid_apuestas, int semid, int n_apostadores, int n_caballos,
   int n_ventanillas, int key, pid_t monitor, pid_t apostador) {
   int msqid, j, k;
   pthread_t *hilos;
   str_ventanilla str;
   apuestas_total apuestas;

   hilos = (pthread_t *) malloc(sizeof(pthread_t) * n_ventanillas);

   if (acceder_shm(shmid_apuestas, (char*) &apuestas) == -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      libera_recursos_gestor(NULL, hilos);
      exit(ERROR);
   }

   apuestas.apostado = (double *) malloc(sizeof(double) * n_caballos);
   apuestas.ganancia = (double *) malloc(sizeof(double) * n_apostadores);
   apuestas.cotizacion = (double *) malloc(sizeof(double) * n_caballos);

   if(crear_cm(&msqid, key) == -1){
      printf("Fallo en el acceso a la cola de mensajes 2.\n");
      libera_recursos_gestor(&apuestas, hilos);
      exit(ERROR);
   }

   str.msqid = msqid;
   str.semid = semid;
   str.apuestas = &apuestas;

   apuestas.total = 1.0 * n_caballos;
   for (j = 0, k = 0; j < n_caballos || k < n_apostadores; j++, k++) {
      apuestas.apostado[j] = 1.0;
      apuestas.cotizacion[j] = apuestas.total;
      apuestas.ganancia[k] = 0.0;
   }

   if (enviar_senal(monitor, SIGUSR1) == -1) {
      printf("Error al enviar senal de gestor a monitor.\n");
      libera_recursos_gestor(&apuestas, hilos);
      exit(ERROR);
   }

   for (j = 0; j < n_ventanillas; j++) {
      if (crear_hilo(&hilos[j], ventanilla, (void*) &str) == -1) {
         printf("Error al crear hilos.\n");
         libera_recursos_gestor(&apuestas, hilos);
         exit(ERROR);
      }
   }

   if (enviar_senal(apostador, SIGUSR1) == -1) {
      printf("Error al enviar senal de gestor a monitor.\n");
      libera_recursos_gestor(&apuestas, hilos);
      exit(ERROR);
   }

   if(pause() != -1){
      printf("Fallo en pause de gestor.\n");
      libera_recursos_gestor(&apuestas, hilos);
      exit(ERROR);
   }

   for (j = 0; j < n_ventanillas; j++) {
      if (cancelar_hilo(hilos[j]) == -1) {
         printf("Error al terminar hilos.\n");
         libera_recursos_gestor(&apuestas, hilos);
         exit(ERROR);
      }

      if (unir_hilo(hilos[j]) == -1) {
         printf("Error al esperar hilos.\n");
         libera_recursos_gestor(&apuestas, hilos);
         exit(ERROR);
      }
   }

   libera_recursos_gestor(&apuestas, hilos);

   exit(OK);
}

/**
 * @brief Funcion que recoge el comportamiento de cada uno de los hilos ventanilla.
 *
 * @param void *args: Argumentos que contienen una estructura de tipo str_ventanilla.
 */
void *ventanilla(void *arg) {
   int apostador, caballo;
   double cantidad;
   str_ventanilla *str;
   apostador_gestor apuesta;

   str = (str_ventanilla*) arg;

   while(1) {
      if (impedir_cancelar() == -1) {
         printf("Error al deshabilitar cancelacion de hilo.\n");
         salir_hilo();
      }

      if(recibir_m(str->msqid, &apuesta, 2) == -1){
         printf("Error al recibir la informacion sobre las tiradas de los caballos.\n");
         salir_hilo();
      }

      apostador = apuesta.nombre[10] - '1';
      if (apostador == 0) {
         if (apuesta.nombre[11] == '\0') {
            apostador = 9;
         } else {
            apostador = 0;
         }
      }
      caballo = apuesta.caballo;
      cantidad = apuesta.apuesta;

      if (Down_Semaforo(str->semid, 0, 0) == -1) {
         printf("Error al ejecutar función Down_Semaforo.\n");
         salir_hilo();
      }

      str->apuestas->total += cantidad;
      str->apuestas->apostado[caballo] += cantidad;
      str->apuestas->cotizacion[caballo] = str->apuestas->total / str->apuestas->apostado[caballo];
      str->apuestas->ganancia[apostador] = cantidad * str->apuestas->cotizacion[caballo];

      if (Up_Semaforo(str->semid, 0, 0) == -1) {
         printf("Error al ejecutar función Up_Semaforo.\n");
         salir_hilo();
      }

      if (permitir_cancelar() == -1) {
         printf("Error al habilitar cancelacion de hilo.\n");
         salir_hilo();
      }
   }
}

void libera_recursos_gestor(apuestas_total *apuestas, pthread_t *hilos) {
   if (apuestas != NULL) {
      if(salir_shm((char*) apuestas) == -1){
         printf("Error al desvincularse de la memoria compartida.\n");
         exit(ERROR);
      }
      free(apuestas->apostado);
      free(apuestas->cotizacion);
      free(apuestas->ganancia);
   }

   if (hilos != NULL) {
      free(hilos);
   }
}
