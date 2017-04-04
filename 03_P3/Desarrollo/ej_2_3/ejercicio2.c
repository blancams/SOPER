/**
 * @brief Sistemas Operativos: Practica 3, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del segundo
 * ejercicio de la tercera practica, que consiste en la creacion
 * de n hijos que compartiran una region de memoria en la que iran sobrescribiendo
 * los datos y el padre leyendolos.
 *
 * @file ejercicio2.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 07-04-2017
 */
 #include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_CHAR 80           /*!< Maximo de caracteres */
#define FILEKEY "/bin/cat"    /*!< Fichero para la generacion de la clave */
#define KEY 1301              /*!< Numero para la generacion de la clave */

/**
 * @brief Estructura que se guardara en la region de memoria compartida.
 *
 * Estructura que contendra un array con un nombre y un int con un id, 
 * y que se almacenara en la memoria compartida.
 */
struct info {
   char nombre[MAX_CHAR];
   int id;
};

/**
 * @brief Manejador para la señal SIGUSR1.
 *
 * Este manejador se encarga de evitar la finalizacion del proceso una vez
 * capturada una señal SIGUSR1.
 *
 * @param sig, numero de la señal.
 */
void manejador(int sig);

/**
 * @brief Generador de numeros aleatorios en un intervalo.
 *
 * num_aleatorio() genera un numero entero aleatorio comprendido entre un minimo
 * y maximo.
 *
 * @param inf, entero limite inferior.
 * @param sup, entero limite superior.
 * @return un entero aleatorio comprendido entre inf y sup.
 */
int num_aleatorio(int inf, int sup);

/**
 * @brief Funcion main del ejercicio2
 *
 * El programa consiste en la creacion de n procesos secuencialmente que compartan,
 * junto con el padre, una region de memoria. Una vez creados, los procesos
 * pediran un nombre por pantalla, lo guardaran en la region compartida y 
 * actualizaran un contador. Cada vez que un hijo termine este proceso, mandara
 * una señal SIGUSR1 al padre, que leera los datos, y terminara.
 *
 * @param argc, numero de argumentos
 * @param argv, array de strings con los argumentos (numero de procesos).
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al introducir los argumentos de entrada, al asignar los
 * manejadores, al ejecutar la funcion fork() o al establecer la region de memoria
 * compartida.
 */
int main(int argc, char *argv[]){
   int key, id;
   int i, n;
   int pid;
   struct info *mens;

   /* Lectura del numero de procesos a crear */
   if(argc != 2){
      printf("Fallo en los argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   }
   n = atoi(argv[1]);

   /* Generacion de la clave */
   key = ftok(FILEKEY, KEY);
   if(key == -1){
      printf("Error al generar la clave.\n");
      exit(EXIT_FAILURE);
   }

   /* Reserva de la region de memoria compartida */
   id = shmget(key, sizeof(mens), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
   if(id == -1){
      printf("Error en la reserva del bloque de memoria.\n");
      exit(EXIT_FAILURE);
   }

   /* Asignacion del manejador */
   if(signal(SIGUSR1, manejador) == SIG_ERR){
      printf("Error en la asignacion del manejador.\n");
      exit(EXIT_FAILURE);
   }

   /* Creacion de los n procesos hijos */
   for(i = 0; i < n; i++){
      if((pid = fork()) == -1){
         printf("Error al hacer el fork.\n");
         exit(EXIT_FAILURE);
      } else if(!pid){
         break;
      }
   }

   /* Vinculacion de los procesos a la region compartida */
   if((mens = (struct info *) shmat(id, NULL, 0)) == NULL){
      printf("Error en el acceso a la memoria compartida.\n");
      shmdt((char *) mens);
      exit(EXIT_FAILURE);
   }

   if(pid){
      /* Codigo del padre */
      mens->id = 0;
      for(i = 0; i < n; i++){
         /* Espera a la señal SIGUSR1 y lectura de datos*/
         if(pause() != -1){
            printf("Error al hacer el pause.\n");
            exit(EXIT_FAILURE);
         }
         printf("Nombre de usuario: %s\tID: %d\n", mens->nombre, mens->id);
         fflush(stdout);
      }
      /* Espera a todos los procesos hijos */
      for(i = 0; i < n; i++){
         wait(NULL);
      }
      /* Desvinculacion de la region de memoria compartida y liberacion de la misma */
      shmdt((char *) mens);
      shmctl(id, IPC_RMID, (struct shmid_ds *) NULL);
   } else if(!pid){
      /* Codigo de los hijos */
      srand(pid);
      sleep(num_aleatorio(i,10));
      /* Lectura de datos */
      printf("\nIntroduzca el nombre de un cliente: \n");
      scanf("%s", mens->nombre);
      mens->id++;
      /* Desvinculacion de la region compartida y envio de la señal al padre */
      shmdt((char *) mens);
      if(kill(getppid(), SIGUSR1) == -1){
         printf("Error en el envio de señal SIGUSR1.\n");
         exit(EXIT_FAILURE);
      }

   }
   exit(EXIT_SUCCESS);
}

void manejador(int sig){
   return;
}

int num_aleatorio(int inf, int sup){
   return rand() % (sup - inf + 1) + inf;
}
