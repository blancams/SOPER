/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * Este modulo consiste en la creacion de una libreria que recoge el
 * comportamiento del gestor de apuestas.
 *
 * @file gestor.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
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

int gestor(int *shmid_apuestas, int semid, int n_apostadores, int n_caballos,
   int n_ventanillas, int key, pid_t monitor, pid_t apostador) {
   int msqid, j, k;
   pthread_t *hilos;
   str_ventanilla str;
   apuestas_total *apuestas;

   /* Reserva de memoria */
   hilos = (pthread_t *) malloc(sizeof(pthread_t) * n_ventanillas);

   /* Acceso a las regiones de memoria compartida de las apuestas */
   if ((apuestas = (apuestas_total *) acceder_shm(shmid_apuestas[0])) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) apuestas);
      libera_recursos_gestor(hilos);
      return ERROR;
   }
   if ((apuestas->apostado = (double *) acceder_shm(shmid_apuestas[1])) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) apuestas->apostado);
      libera_recursos_gestor(hilos);
      return ERROR;
   }
   if ((apuestas->ganancia = (double **) acceder_shm(shmid_apuestas[2])) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) apuestas->ganancia);
      libera_recursos_gestor(hilos);
      return ERROR;
   }
   if ((apuestas->cotizacion = (double *) acceder_shm(shmid_apuestas[3])) == (void *) -1) {
      printf("Error al acceder a memoria compartida en gestor.\n");
      salir_shm((void *) apuestas->cotizacion);
      libera_recursos_gestor(hilos);
      return ERROR;
   }
   for (j = 4; j < n_apostadores + 4; j++) {
      if ((apuestas->ganancia[j-4] = (double *) acceder_shm(shmid_apuestas[j])) == (void *) -1) {
         printf("Error al acceder a memoria compartida en gestor.\n");
         salir_shm((void *) apuestas->ganancia[j-4]);
         libera_recursos_gestor(hilos);
         return ERROR;
      }
   }

   /* Acceso a la cola de mensajes */
   if(crear_cm(&msqid, key) == -1){
      printf("Fallo en el acceso a la cola de mensajes 2.\n");
      libera_recursos_gestor(hilos);
      return ERROR;
   }

   /* Inicializacion de los datos */
   str.msqid = msqid;
   str.semid = semid;
   str.apuestas = apuestas;

   apuestas->total = 1.0 * n_caballos;
   for (j = 0; j < n_caballos; j++) {
      apuestas->apostado[j] = 1.0;
      apuestas->cotizacion[j] = apuestas->total;
   }
   for (k = 0; k < n_apostadores; k++) {
      for (j = 0; j < n_caballos; j++) {
         apuestas->ganancia[k][j] = 0.0;
      }
   }

   /* Envio de la señal al proceso monitor, indicando la inicializacion de los datos */
   if (enviar_senal(monitor, SIGUSR1) == -1) {
      printf("Error al enviar senal de gestor a monitor.\n");
      libera_recursos_gestor(hilos);
      return ERROR;
   }

   /* Creacion de los hilos ventanilla */
   for (j = 0; j < n_ventanillas; j++) {
      if (crear_hilo(&hilos[j], ventanilla, (void*) &str) == -1) {
         printf("Error al crear hilos.\n");
         libera_recursos_gestor(hilos);
         return ERROR;
      }
   }

   /* Envio de la señal al proceso apostador de que las ventanillas ya estan disponibles */
   if (enviar_senal(apostador, SIGUSR1) == -1) {
      printf("Error al enviar senal de gestor a monitor.\n");
      libera_recursos_gestor(hilos);
      return ERROR;
   }

   /* Espera del gestor al inicio de la carrera */
   if(pause() != -1){
      printf("Fallo en pause de gestor.\n");
      libera_recursos_gestor(hilos);
      return ERROR;
   }

   /* Cancelacion de las ventanillas antes de la carrera */
   for (j = 0; j < n_ventanillas; j++) {
      if (cancelar_hilo(hilos[j]) == -1) {
         printf("Error al terminar hilos.\n");
         libera_recursos_gestor(hilos);
         return ERROR;
      }

      if (unir_hilo(hilos[j]) == -1) {
         printf("Error al esperar hilos.\n");
         libera_recursos_gestor(hilos);
         return ERROR;
      }
   }

   /* Liberacion de recursos */
   libera_recursos_gestor(hilos);

   return OK;
}

void *ventanilla(void *arg) {
   int apostador, caballo;
   double cantidad;
   str_ventanilla *str;
   apostador_gestor apuesta;

   str = (str_ventanilla*) arg;

   while(1) {
      usleep(1000);

      /* Recepcion del mensaje de apuesta */
      if(recibir_m(str->msqid, (void *) &apuesta, 0, sizeof(apostador_gestor) - sizeof(long)) == -1){
         printf("Error al recibir la informacion sobre las tiradas de los caballos.\n");
         salir_hilo();
      }
      /* Impedimos cancelar el hilo en mitad de su ejecucionn */
      if (impedir_cancelar() == -1) {
         printf("Error al deshabilitar cancelacion de hilo.\n");
         salir_hilo();
      }

      /* Asignacion de los datos */
      apostador = apuesta.ap.nombre[10] - '1';
      if (apostador == 0) {
         if (apuesta.ap.nombre[11] != '0') {
            apostador = 0;
         } else {
            apostador = 9;
         }
      }
      caballo = apuesta.ap.caballo;
      cantidad = apuesta.ap.apuesta;

      /* Modificacion de las apuestas controlado por semaforos */
      if (Down_Semaforo(str->semid, 0, 0) == -1) {
         printf("Error al ejecutar función Down_Semaforo.\n");
         salir_hilo();
      }

      str->apuestas->total += cantidad;
      str->apuestas->apostado[caballo] += cantidad;
      str->apuestas->cotizacion[caballo] = str->apuestas->total / str->apuestas->apostado[caballo];
      str->apuestas->ganancia[apostador][caballo] += cantidad * (str->apuestas->cotizacion[caballo] - 1);

      if (Up_Semaforo(str->semid, 0, 0) == -1) {
         printf("Error al ejecutar función Up_Semaforo.\n");
         salir_hilo();
      }
      /* Permitimos cancelar el hilo */
      if (permitir_cancelar() == -1) {
         printf("Error al habilitar cancelacion de hilo.\n");
         salir_hilo();
      }
   }
}

void libera_recursos_gestor(pthread_t *hilos) {
   if (hilos != NULL) {
      free(hilos);
   }
}
