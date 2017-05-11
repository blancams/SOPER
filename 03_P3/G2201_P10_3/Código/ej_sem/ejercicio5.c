/**
 * @brief Sistemas Operativos: Practica 3, ejercicio 5
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del quinto
 * ejercicio de la tercera practica, que consiste en la creacion
 * de un cliente de pruebas para las funciones implementadas en el cuarto ejercicio.
 *
 * @file ejercicio5.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 07-04-2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>

#include "../semaforos.h"

#define SEMKEY 71458       /*!< Numero para la clave del semaforo */
#define N_SEMAFOROS 5      /*!< Numero de semaforos */

/**
 * @brief Funcion main del ejercicio5
 *
 * El programa consiste en la creacion de un numero de semaforos y su 
 * manipulacion para comprobar la correccion de las funciones implementadas
 * en el ejercicio 4.
 *
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al reservar memoria o al emplear las funciones 
 * implementadas sobre semaforos.
 */
int main(void) {
   int i, semid, *active;
   union semun arg;

   /* Reserva de memoria y asignacion de datos */
   active = (int*) malloc((N_SEMAFOROS-1)*sizeof(int));
   arg.array = (unsigned short*) malloc(N_SEMAFOROS*sizeof(unsigned short));

   if (active == NULL || arg.array == NULL) {
      printf("Error al reservar memoria.\n");
      exit(EXIT_FAILURE);
   }

   for (i = 0; i < N_SEMAFOROS; i++) {
      if (i < (N_SEMAFOROS-1)) {
         active[i] = i + 1;
      }
      arg.array[i] = 1;
   }

   /* Creacion del array de semaforos */
   if (Crear_Semaforo(SEMKEY, N_SEMAFOROS, &semid) == -1) {
      printf("Error al crear los semáforos.\n");
      exit(EXIT_FAILURE);
   }

   printf("Creación correcta. ID %d.\n", semid);

   /* Inicializacion de los semaforos */
   if (Inicializar_Semaforo(semid, arg.array) == -1) {
      printf("Error al inicializar los semáforos.\n");
      exit(EXIT_FAILURE);
   }

   printf("Inicialización correcta.\n");

   /* Down del primer semaforo */
   if (Down_Semaforo(semid, 0, 1) == -1) {
      printf("Error al ejecutar función Down_Semaforo.\n");
      exit(EXIT_FAILURE);
   }

   printf("Down_Semaforo correcto (0).\n");

   /* Comprobacion de los valores de los semaforos */
   if (semctl(semid, N_SEMAFOROS, GETALL, arg) == -1) {
      printf("Error al ejecutar función semctl.\n");
      exit(EXIT_FAILURE);
   }

   for (i = 0; i < N_SEMAFOROS; i++) {
      printf("Valor del semáforo %d: %d\n", i, arg.array[i]);
   }

   /* Down de los otros semaforos */
   if (DownMultiple_Semaforo(semid, N_SEMAFOROS-1, 1, active) == -1) {
      printf("Error al ejecutar función DownMultiple_Semaforo.\n");
      exit(EXIT_FAILURE);
   }

   printf("DownMultiple_Semaforo correcto (1-4).\n");

   /* Comprobacion de los valores de los semaforos */
   if (semctl(semid, N_SEMAFOROS, GETALL, arg) == -1) {
      printf("Error al ejecutar función semctl.\n");
      exit(EXIT_FAILURE);
   }

   for (i = 0; i < N_SEMAFOROS; i++) {
      printf("Valor del semáforo %d: %d\n", i, arg.array[i]);
   }

   /* Up del primer semaforo */
   if (Up_Semaforo(semid, 0, 1) == -1) {
      printf("Error al ejecutar función Up_Semaforo.\n");
      exit(EXIT_FAILURE);
   }

   printf("Up_Semaforo correcto (0).\n");

   /* Comprobacion de los valores de los semaforos */
   if (semctl(semid, N_SEMAFOROS, GETALL, arg) == -1) {
      printf("Error al ejecutar función semctl.\n");
      exit(EXIT_FAILURE);
   }

   for (i = 0; i < N_SEMAFOROS; i++) {
      printf("Valor del semáforo %d: %d\n", i, arg.array[i]);
   }

   /* Up de los demas semaforos */
   if (UpMultiple_Semaforo(semid, N_SEMAFOROS-1, 1, active) == -1) {
      printf("Error al ejecutar función UpMultiple_Semaforo.\n");
      exit(EXIT_FAILURE);
   }

   printf("UpMultiple_Semaforo correcto (1-4).\n");

   /* Comprobacion de los valores de los semaforos */
   if (semctl(semid, N_SEMAFOROS, GETALL, arg) == -1) {
      printf("Error al ejecutar función semctl.\n");
      exit(EXIT_FAILURE);
   }

   for (i = 0; i < N_SEMAFOROS; i++) {
      printf("Valor del semáforo %d: %d\n", i, arg.array[i]);
   }

   /* Liberacion de recursos */
   if (Borrar_Semaforo(semid) == -1) {
      printf("Error al borrar los semáforos.\n");
      exit(EXIT_FAILURE);
   }

   printf("Borrado correcto.\n");

   free(active);
   free(arg.array);

   exit(EXIT_SUCCESS);
}
