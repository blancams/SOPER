#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>

#include "../semaforos.h"

#define SEMKEY 71458
#define N_SEMAFOROS 5

int main(void) {
   int i, semid, *active;
   union semun arg;

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

   if (Crear_Semaforo(SEMKEY, N_SEMAFOROS, &semid) == -1) {
      printf("Error al crear los semáforos.\n");
      exit(EXIT_FAILURE);
   }

   printf("Creación correcta. ID %d.\n", semid);

   if (Inicializar_Semaforo(semid, arg.array) == -1) {
      printf("Error al inicializar los semáforos.\n");
      exit(EXIT_FAILURE);
   }

   printf("Inicialización correcta.\n");

   if (Down_Semaforo(semid, 0, 1) == -1) {
      printf("Error al ejecutar función Down_Semaforo.\n");
      exit(EXIT_FAILURE);
   }

   printf("Down_Semaforo correcto (0).\n");

   if (semctl(semid, N_SEMAFOROS, GETALL, arg) == -1) {
      printf("Error al ejecutar función semctl.\n");
      exit(EXIT_FAILURE);
   }

   for (i = 0; i < N_SEMAFOROS; i++) {
      printf("Valor del semáforo %d: %d\n", i, arg.array[i]);
   }

   if (DownMultiple_Semaforo(semid, N_SEMAFOROS-1, 1, active) == -1) {
      printf("Error al ejecutar función DownMultiple_Semaforo.\n");
      exit(EXIT_FAILURE);
   }

   printf("DownMultiple_Semaforo correcto (1-4).\n");

   if (semctl(semid, N_SEMAFOROS, GETALL, arg) == -1) {
      printf("Error al ejecutar función semctl.\n");
      exit(EXIT_FAILURE);
   }

   for (i = 0; i < N_SEMAFOROS; i++) {
      printf("Valor del semáforo %d: %d\n", i, arg.array[i]);
   }

   if (Up_Semaforo(semid, 0, 1) == -1) {
      printf("Error al ejecutar función Up_Semaforo.\n");
      exit(EXIT_FAILURE);
   }

   printf("Up_Semaforo correcto (0).\n");

   if (semctl(semid, N_SEMAFOROS, GETALL, arg) == -1) {
      printf("Error al ejecutar función semctl.\n");
      exit(EXIT_FAILURE);
   }

   for (i = 0; i < N_SEMAFOROS; i++) {
      printf("Valor del semáforo %d: %d\n", i, arg.array[i]);
   }

   if (UpMultiple_Semaforo(semid, N_SEMAFOROS-1, 1, active) == -1) {
      printf("Error al ejecutar función UpMultiple_Semaforo.\n");
      exit(EXIT_FAILURE);
   }

   printf("UpMultiple_Semaforo correcto (1-4).\n");

   if (semctl(semid, N_SEMAFOROS, GETALL, arg) == -1) {
      printf("Error al ejecutar función semctl.\n");
      exit(EXIT_FAILURE);
   }

   for (i = 0; i < N_SEMAFOROS; i++) {
      printf("Valor del semáforo %d: %d\n", i, arg.array[i]);
   }

   if (Borrar_Semaforo(semid) == -1) {
      printf("Error al borrar los semáforos.\n");
      exit(EXIT_FAILURE);
   }

   printf("Borrado correcto.\n");

   exit(EXIT_SUCCESS);
}
