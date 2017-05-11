/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del segundo
 * ejercicio de la cuarta practica, que consiste en la simulacion de
 * una carrera de caballos.
 *
 * @file ejercicio_final.c
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
#include "../../recursos/memcomp.h"
#include "../../recursos/mensajes.h"
#include "../../recursos/hilos.h"
#include "../../recursos/semaforos.h"
#include "caballo.h"
#include "gestor.h"
#include "apostador.h"
#include "monitor.h"

#define N_KEY_MENSAJES 300       /*!< Numero para la generacion de la clave de la cola de mensajes */
#define N_KEY_ACCGTAPU 275       /*!< Numero para la generacion de la memoria compartida ente gestor y apostador */
#define N_KEY_POSICION 334       /*!< Numero para la generacion de la memoria compartida ente monitor y principal */
#define N_KEY_SEMAFORO 72345     /*!< Numero para la generacion del semaforo */

/**
 * @brief Manejador de la señal SIGUSR1.
 *
 * @param int sig: Señal
 */
void manejador_SIGUSR1(int sig);

/**
 * @brief Manejador de la señal SIGALRM.
 *
 * @param int sig: Señal
 */
void manejador_SIGALRM(int sig);

void libera_recursos(int *shmid_apuestas, int *shmid_posiciones, int *semid,
   int *msqid, int *pid_procesos, int *posiciones, int *tuberias, int n,
      apuestas_total *apuestas);

/**
 * @brief Funcion main del ejercicio final
 *
 * El programa consiste en la creacion de los procesos monitor, gestor de apuestas, generador
 * de puestas y los caballos. Mediante estos procesos y diversos recursos (semaforos, memoria
 * compartida, colas de mensajes, creacion de hilos, señales, etc) se simula una carrera
 * de caballos. El programa termina cuando el usuario pulza Ctrl+C o un caballo llega a la meta.
 *
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al reservar recursos o al liberarlos.
 */
int main(int argc, char *argv[]){

   int i, j;
   int n_caballos, longitud, n_apostadores, n_ventanillas;
   int pid, msqid, shmid_apuestas, shmid_posiciones, semid, signvalue;
   int fd[2], *tuberias, *posiciones;
   unsigned short semval;
   sigset_t sset;
   pid_t *pid_procesos;
   caballo_principal mensaje;
   apuestas_total apuestas;

   /* Comprobación de los argumentos de entrada */
	if(argc != 5){
      printf("Fallo en el número de argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   } else {
      for (i = 1; i < argc; i++) {
         for (j = 0; argv[i][j] != '\0'; j++) {
            if ((argv[i][j] < 48) || (argv[i][j] > 57)) {
               printf("Fallo en los argumentos de entrada.\n");
               exit(EXIT_FAILURE);
            }
         }
      }
   }

   /* Asignacion de los argumentos de entrada y comprobacion de validez */
   n_caballos = atoi(argv[1]);
   if(n_caballos <= 0 || n_caballos > 10){
      printf("Fallo en los argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   }

   longitud = atoi(argv[2]);
   if(longitud <= 0){
      printf("Fallo en los argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   }

   n_apostadores = atoi(argv[3]);
   if(n_apostadores <= 0 || n_apostadores > 10){
      printf("Fallo en los argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   }

   n_ventanillas = atoi(argv[4]);
   if(n_ventanillas <= 0){
      printf("Fallo en los argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   }

   printf("Pasa de asignaciones.\n");

   /* Asignacion del manejador de alarma */
   if (crear_manej(SIGUSR1, &manejador_SIGUSR1) == -1) {
      printf("Error al crear el manejador.\n");
      exit(EXIT_FAILURE);
   }

   /* Asignacion del manejador de alarma */
   if (crear_manej(SIGALRM, &manejador_SIGALRM) == -1) {
      printf("Error al crear el manejador.\n");
      exit(EXIT_FAILURE);
   }

   printf("Pasa de manejadores.\n");

   /* No tengo ni zorra de lo que estoy haciendo */
   /* (Fer) Pues bastante idea tenías. He añadido todas las cosas extra que requieren
   reserva de memoria: los hilos y los arrays con datos de apuestas. Faltan. */
   pid_procesos = (pid_t *) malloc(sizeof(pid_t) * (n_caballos+3));
   posiciones = (int *) malloc(sizeof(int) * n_caballos);
   tuberias = (int *) malloc(sizeof(int) * n_caballos * 2);

   apuestas.apostado = (double *) malloc(sizeof(double) * n_caballos);
   apuestas.ganancia = (double *) malloc(sizeof(double) * n_apostadores);
   apuestas.cotizacion = (double *) malloc(sizeof(double) * n_caballos);

   printf("Pasa de reserva de memoria.\n");

   /* Creacion de la cola de mensajes para que los caballos le pasen
      el resultado de sus tiradas al proceso principal y para la comunicacion
      entre gestor de apuestas y apostador */
   if(crear_cm(&msqid, N_KEY_MENSAJES)== -1){
      printf("Fallo en la creacion de la cola de mensajes 1.\n");
      libera_recursos(NULL, NULL, NULL, NULL, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
      exit(EXIT_FAILURE);
   }

   printf("Pasa de creación de cola de mensajes.\n");

   /* (Fer) Creacion de la memoria compartida para que gestor de apuestas y
      monitor tengan la informacion de las apuestas */
   if(crear_shm(sizeof(apuestas), &shmid_apuestas, N_KEY_ACCGTAPU) == -1) {
      printf("Fallo en la creacion de memoria compartida.\n");
      libera_recursos(NULL, NULL, NULL, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
      exit(EXIT_FAILURE);
   }

   printf("Pasa de primera memoria compartida.\n");

   if(crear_shm(sizeof(posiciones), &shmid_posiciones, N_KEY_POSICION) == -1) {
      printf("Fallo en la creacion de memoria compartida.\n");
      libera_recursos(&shmid_apuestas, NULL, NULL, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
      exit(EXIT_FAILURE);
   }

   printf("Pasa de segunda memoria compartida.\n");

   /* (Fer) Creacion del array de semaforos (de momento con uno solo, puede que
      acabe habiendo mas). */
   if (Crear_Semaforo(N_KEY_SEMAFORO, 1, &semid) == -1) {
      printf("Error al crear los semáforos.\n");
      libera_recursos(&shmid_apuestas, &shmid_posiciones, NULL, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
      exit(EXIT_FAILURE);
   }

   printf("Pasa de crear semaforo.\n");

   /* Inicializacion del semaforo */
   semval = 1;
   if (Inicializar_Semaforo(semid, &semval) == -1) {
      printf("Error al inicializar los semáforos.\n");
      libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
      exit(EXIT_FAILURE);
   }

   printf("Pasa de inicializar semaforo.\n");

   for (i = 0; i < n_caballos + 3; i++) {
      pid_procesos[i] = getpid();
   }

   /* Creacion de los procesos */
   /* (Fer) He sacado del bucle casi todo. He dejado la creacion de las tuberias (no hacia
      falta el array como hablamos en clase, se hace asi, creando una antes de cada
      fork, no me acordaba), que los caballos hijos cierren la escritura de su tuberia,
      y guardo todos los procesos en el array de pid. */
   for(i = 0; i < n_caballos + 3; i++){

      if (i > 2) {

         if(pipe(fd)==-1){
            printf("Error creando la tubería.\n");
            libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
            exit(EXIT_FAILURE);
         } else {
            printf("Lo hace.\n");
            printf("%d %d\n", fd[0], fd[1]);
         }

      }

      if((pid = fork()) == -1) {

         printf("Error al hacer el fork.\n");
         libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
         exit(EXIT_FAILURE);

      } else if(!pid){

         printf("Creado proceso %d.\n", getpid());

         if (i > 2) {
            close(fd[1]);
         }

         break;

      } else {

         pid_procesos[i] = pid;
         if (i > 2) {
            tuberias[i-3] = fd[0];
            tuberias[i-2] = fd[1];
         }
         close(fd[0]);

      }

   }

   if (i == 0) {

      printf("Crea monitor.\n");

      if (monitor(shmid_apuestas, shmid_posiciones, n_caballos) == -1) {
         printf("Error en monitor.\n");
         libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
         exit(EXIT_FAILURE);
      }

   } else if (i == 1) {

      printf("Crea apostador.\n");

      if (apostador(N_KEY_MENSAJES, n_apostadores, n_caballos) == -1) {
         printf("Error en apostador.\n");
         libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
         exit(EXIT_FAILURE);
      }

   } else if (i == 2) {

      printf("Crea gestor.\n");

      if (gestor(shmid_apuestas, semid, n_apostadores, n_caballos, n_ventanillas, N_KEY_MENSAJES, pid_procesos[0], pid_procesos[1]) == -1) {
         printf("Error en gestor.\n");
         libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
         exit(EXIT_FAILURE);
      }

   } else if (i == n_caballos + 3) {
      /* Proceso principal */
      /* (Fer) Aqui he: cerrado la tuberia y creado un bucle para la simulacion, donde
         basicamente he copiado lo que tu ya tenias, cambiando algunas cosas para adaptar
         los cambios hechos en el resto del codigo. Hay que arreglar la parte de la
         liberacion de recursos pero meh, al final. */

      printf("Hace algo en principal.\n");

      if (acceder_shm(shmid_posiciones, (char*) posiciones) == -1) {
         printf("Error al acceder a memoria compartida en gestor.\n");
         libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
         exit(EXIT_FAILURE);
      }

      if (crear_mascara(&sset, SIGINT) == -1) {
         printf("Error al crear la mascara de senales.\n");
         libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
         exit(EXIT_FAILURE);
      }

      /* Inicializacion de las posiciones de cada caballo ? */
      for(j = 0; j < n_caballos; j++){
         posiciones[j] = 0;
      }

      if (alarm(15) == -1) {
         printf("Fallo al crear alarma.\n");
         libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
         exit(EXIT_FAILURE);
      }

      if (pause() != -1) {
         printf("Fallo en pause de gestor.\n");
         return -1;
      }

      waitpid(pid_procesos[1], NULL, 0);
      waitpid(pid_procesos[2], NULL, 0);

      while(1) {
         /* Escribimos para cada caballo? No tiene sentido esto no ?  No me acuerdo de tuberias sorry */
         for (j = 1; j < n_caballos*2; j = j + 2) {
            write(fd[j], posiciones, sizeof(posiciones));
         }

         /* Le mandamos una señal a cada caballo ?  Como ?  Array de pids guarro ? */
         for(j = 3; j < n_caballos+3; j++){
            if(enviar_senal(pid_procesos[j], SIGUSR1) == -1){
               printf("Fallo al enviar la señal desde el proceso principal a los caballos.\n");
               libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
               exit(EXIT_FAILURE);
            }
         }

         for(j = 0; j < n_caballos; j++){
            if(recibir_m(msqid, &mensaje, 1) == -1){
               printf("Error al recibir la informacion sobre las tiradas de los caballos.\n");
               libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
               exit(EXIT_FAILURE);
            }
            posiciones[j] += mensaje.tirada;
         }

         /* Algun caballo ha llegado ya a la meta ? Este ejercicio no tiene sentido en eficiencia
         (o lo estoy haciendo tremendamente mal) */
         for(j = 0; j < n_caballos; j++){
            if(posiciones[j] >= longitud) {
            /* Pues terminamos */
            break;
            }
         }

         /* Aqui tiene que venir la captura de SIGINT */
         if (senal_bloqueada(SIGINT, &signvalue) == -1) {
            printf("Error al comprobar si se ha detectado la senal.\n");
            libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
            exit(EXIT_FAILURE);
         }

         if (signvalue) {
            break;
         }

         if (enviar_senal(pid_procesos[0], SIGUSR1) == -1) {
            printf("Fallo al enviar la señal desde el proceso principal al monitor.\n");
            libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
            exit(EXIT_FAILURE);
         }
      }

      libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);

   } else {

      printf("Crea caballo %d.\n", i-3);

      if (caballo(i-3, fd[0], posiciones, n_caballos, N_KEY_MENSAJES) == -1){
         printf("Fallo en caballos %d.\n", i);
         libera_recursos(&shmid_apuestas, &shmid_posiciones, &semid, &msqid, pid_procesos, posiciones, tuberias, n_caballos+3, &apuestas);
         exit(EXIT_FAILURE);
      }

   }

   exit(EXIT_SUCCESS);

}

void manejador_SIGUSR1(int sig){
   return;
}

void manejador_SIGALRM(int sig){
   return;
}

void libera_recursos(int *shmid_apuestas, int *shmid_posiciones, int *semid,
   int *msqid, int *pid_procesos, int *posiciones, int *tuberias, int n,
      apuestas_total *apuestas) {
   int i;

   printf("Entra aquí\n");

   if (shmid_apuestas != NULL) {
      if(eliminar_shm(*shmid_apuestas) == -1){
         printf("Error al eliminar la region de memoria compartida.\n");
         exit(EXIT_FAILURE);
      }
   }
   if (shmid_posiciones != NULL) {
      if(eliminar_shm(*shmid_posiciones) == -1){
         printf("Error al eliminar la region de memoria compartida.\n");
         exit(EXIT_FAILURE);
      }
   }
   if (semid != NULL) {
      if(Borrar_Semaforo(*semid) == -1){
         printf("Error al eliminar el semaforo.\n");
         exit(EXIT_FAILURE);
      }
   }
   if (msqid != NULL) {
      if(eliminar_cm(*msqid) == -1){
         printf("Error al eliminar la cola de mensajes.\n");
         exit(EXIT_FAILURE);
      }
   }

   if (apuestas != NULL) {
      free(apuestas->apostado);
      free(apuestas->ganancia);
      free(apuestas->cotizacion);
   }

   if (pid_procesos != NULL) {
      for (i = 0; i < n; i++) {
         if (pid_procesos[i] != getpid()) {
            if (enviar_senal(pid_procesos[i], SIGTERM) == -1) {
               printf("Error al terminar proceso %d.\n", i);
               exit(EXIT_FAILURE);
            }
         }
      }
      free(pid_procesos);
   }
   if (posiciones != NULL) {
      free(posiciones);
   }
   if (tuberias != NULL) {
      free(tuberias);
   }
}
