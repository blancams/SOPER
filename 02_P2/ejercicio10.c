/**
 * @brief Sistemas Operativos: Practica 2, ejercicio 10
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del decimo
 * ejercicio de la segunda practica, que consiste en el manejo de
 * comunicacion entre dos procesos a base de señales y utilizando
 * mascaras y manejadores.
 *
 * @file ejercicio10.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 17-03-2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#define NUM_WRD 13      /*!< Numero de palabras de la frase */
#define MAX_CHAR 10     /*!< Maximo de caracteres para palabras de la frase */
#define NUM_STR 50      /*!< Numero de palabras que lee el proceso B */
#define SENTENCE {"EL", "PROCESO", "A", "ESCRIBE", "EN", "UN", "FICHERO", "HASTA", "QUE", "LEE", "LA", "CADENA", "FIN"} /*!< Frase */

/**
 * @brief Funcion que genera un numero aleatorio
 *
 * randInt genera un numero entero aleatorio entre 0 y range-1.
 * @param range, maximo para el numero aleatorio
 * @return int, el numero aleatorio generado
 */
int randInt(int range);

/**
 * @brief Manejador 1: despertar
 *
 * Manejador que despierta al proceso A despues de que haya leido FIN
 * a traves de la señal SIGUSR1
 * @param sig, numero de la señal
 */
void despertar(int sig);

/**
 * @brief Manejador 1: alarma
 *
 * Manejador que sirve para temporizar las lecturas del proceso B a traves
 * de alarm() y SIGALRM.
 * @param sig, numero de la señal
 */
void alarma(int sig);

/**
 * @brief Manejador 1: morir
 *
 * Manejador que mata al proceso A cuando el proceso B ha leido NUM_STR palabras.
 */
void morir(int sig);

/**
 * @brief Funcion main del ejercicio10
 *
 * El programa consiste en la creacion de un proceso padre y uno hijo. El hijo
 * escribe en un fichero palabras de SENTENCE hasta que escribe FIN, momento en
 * el que se bloquea, con una mascara que le hara desbloquearse solo si llegan
 * las señales SIGUSR1, SIGALRM o SIGTERM. Mientras tanto, B lee cada 5 segundos
 * una palabra del fichero, y cuando lee FIN desbloquea el proceso A para volver
 * a repetir la operacion hasta que B haya leido NUM_STR palabras, momento en el
 * que B manda una señal para que muera A y luego B muere.
 *
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al crear los manejadores, al crear la mascara, al hacer
 * el fork() o al abrir/cerrar el fichero.
 */
int main() {
   int pid, nread;
   char frase[NUM_WRD][MAX_CHAR] = SENTENCE;
   char palabra[MAX_CHAR];
   FILE *file;
   sigset_t set, oset;
   void despertar();
   void alarma();
   void morir();

   /* Para la generacion de numeros aleatorios */
   srand(time(NULL));

   /* Asignacion de los manejadores */
   if (signal(SIGUSR1, despertar) == SIG_ERR) {
      printf("Error al crear el manejador.\n");
      exit(EXIT_FAILURE);
   } else if (signal(SIGALRM, alarma) == SIG_ERR) {
      printf("Error al crear el manejador.\n");
      exit(EXIT_FAILURE);
   } else if (signal(SIGTERM, morir) == SIG_ERR) {
      printf("Error al crear el manejador.\n");
      exit(EXIT_FAILURE);
   }

   /* Creacion de la mascara para este proceso */
   if (sigemptyset(&set) == -1) {
      printf("Error al vaciar la máscara.\n");
      exit(EXIT_FAILURE);
   } else if (sigaddset(&set, SIGUSR1) == -1) {
      printf("Error al añadir una señal a la máscara.\n");
      exit(EXIT_FAILURE);
   } else if (sigaddset(&set, SIGALRM) == -1) {
      printf("Error al añadir una señal a la máscara.\n");
      exit(EXIT_FAILURE);
   } else if (sigaddset(&set, SIGTERM) == -1) {
      printf("Error al añadir una señal a la máscara.\n");
      exit(EXIT_FAILURE);
   } else if (sigprocmask(SIG_SETMASK, &set, &oset) == -1) {
      printf("Error al entregar la máscara al proceso.\n");
      exit(EXIT_FAILURE);
   }

   /* Creacion de proceso A */
   if ((pid = fork()) == -1) {
      printf("Error al hacer el fork().\n");
      exit(EXIT_FAILURE);
   } else if (pid == 0) {
      /* Abre el fichero y entra en el bucle para escribir palabras;
      cuando llega a FIN, cierra el fichero, entra en suspension, y cuando
      vuelve a despertar, vuelve a abrir el fichero para escribir de nuevo */
      file = fopen("fichero.txt", "w+");
      if (file == NULL) {
         printf("Error al abrir el fichero\n");
         exit(EXIT_FAILURE);
      }

      while(1) {
         strcpy(palabra, frase[randInt(NUM_WRD)]);
         fprintf(file, "%s ", palabra);
         if (strcmp("FIN", palabra) == 0) {
            fclose(file);
            sigsuspend(&oset);
            file = fopen("fichero.txt", "w+");
            if (file == NULL) {
               printf("Error al abrir el fichero\n");
               exit(EXIT_FAILURE);
            }
         }
      }
   } else {
      /* Abre el fichero para leer, entra en el bucle que se acaba cuando llega
      a NUM_STR palabras, y cada cinco segundos realiza una lectura; si se lee
      FIN, se cierra el fichero, se manda la señal a A para que escriba de nuevo,
      se hace una pausa para dar tiempo a la escritura, y se vuelve a abrir el
      fichero para leer en la siguiente iteracion */
      file = fopen("fichero.txt", "r");
      if (file == NULL) {
         printf("Error al abrir el fichero\n");
         exit(EXIT_FAILURE);
      }

      while (nread < NUM_STR) {
         alarm(5);
         sigsuspend(&oset);

         fflush(stdout);
         fscanf(file, "%s", palabra);
         fprintf(stdout, "%s\n", palabra);

         if (strcmp("FIN", palabra) == 0) {
            fclose(file);
            kill(pid, SIGUSR1);
            sleep(1);
            file = fopen("fichero.txt", "r");
            if (file == NULL) {
               printf("Error al abrir el fichero\n");
               exit(EXIT_FAILURE);
            }
         }

         nread++;
      }

      /* Cuando acaba el bucle, manda la señal a A para que muera y cierra el archivo
      por si no lo estuviera */
      kill(pid, SIGTERM);
      fclose(file);
   }

   /* Tras terminar toda la tarea, B muere */
   printf("Proceso B muere.\n");
   exit(EXIT_SUCCESS);
}

void despertar(int sig) {
   return;
}

void alarma(int sig) {
   printf("Proceso B lee una palabra: ");
}

void morir(int sig) {
   printf("Proceso A muere.\n");
   exit(EXIT_SUCCESS);
}

int randInt(int range) {
   return rand()%range;
}
