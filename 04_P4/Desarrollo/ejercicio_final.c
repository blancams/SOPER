
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "senales/senales.h"
#include "memcomp/memcomp.h"
#include "mensajes/mensajes.h"


#define N_KEY_CABALLOS 300
#define N_KEY_APUESTAS 247


void manejador_SIGTERM(int sig);
void manejador_SIGUSR1(int sig);

int tirada_normal();
int tirada_ganadora();
int tirada_remontadora();

/* Estructura para la comunicacion de los caballos al proceso principal */
typedef struct _Caballos_Principal{
   long tipo;
   int tirada;
} caballo_principal;

/* Estructura para la comunicacion del generador de apuestas al gestor */
typedef struct _Apostador_Gestor{
   long tipo;
   char nombre[20];
   int caballo;
   double apuesta;
} apostador_gestor;


int main(int argc, char *argv[]){

   int i, j;
   int n_caballos, longitud, n_apostadores, n_ventanillas;
   int pid;
   int msqid_tiradas, msqid_apuestas;
   int fd[2];
   pid_t * pid_caballos;
   int *posiciones;
   caballo_principal mensaje;

   /* Comprobación de los argumentos de entrada */
	if(argc != 5){
      printf("Fallo en el número de argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   } else {
      i = 0;
      while(argv[1][i] != '\0') {
         if ((argv[1][i] < 48) || (argv[1][i] > 57)) {
            printf("Fallo en los argumentos de entrada.\n");
            exit(EXIT_FAILURE);
         }
         i++;
      }
      while(argv[2][i] != '\0') {
         if ((argv[1][i] < 48) || (argv[1][i] > 57)) {
            printf("Fallo en los argumentos de entrada.\n");
            exit(EXIT_FAILURE);
         }
         i++;
      }
      while(argv[3][i] != '\0') {
         if ((argv[1][i] < 48) || (argv[1][i] > 57)) {
            printf("Fallo en los argumentos de entrada.\n");
            exit(EXIT_FAILURE);
         }
         i++;
      }
      while(argv[4][i] != '\0') {
         if ((argv[1][i] < 48) || (argv[1][i] > 57)) {
            printf("Fallo en los argumentos de entrada.\n");
            exit(EXIT_FAILURE);
         }
         i++;
      }
   }



   /* Asignacion de los argumentos de entrada y comprobacion de validez */
   n_caballos = atoi(argv[1]);
   if(n_caballos > 10){
      printf("Fallo en los argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   }
   longitud = atoi(argv[2]);
   //////////////////////////////////////////// No se si te parece bien comprobar esto
   if(longitud <= 0){
      printf("Fallo en los argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   }
   n_apostadores = atoi(argv[3]);
   if(n_apostadores > 10){
      printf("Fallo en los argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   }
   //////////////////////////////////////////// No se si te parece bien comprobar esto
   n_ventanillas = atoi(argv[4]);
   if(n_ventanillas <= 0){
      printf("Fallo en los argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   }



   /* Asignacion del manejador de terminacion */
   if (crear_manej(SIGTERM, &manejador_SIGTERM) == -1) {
      printf("Error al crear el manejador.\n");
      exit(EXIT_FAILURE);
   }

   /* Asignacion del manejador de alarma */
   if (crear_manej(SIGUSR1, &manejador_SIGUSR1) == -1) {
      printf("Error al crear el manejador.\n");
      exit(EXIT_FAILURE);
   }


   ///////////////////////////////////// Usamos la misma cola??? Turbio como
   ///////////////////////////////////// vamos a crearlas....
   /* Creacion de la cola de mensajes para que los caballos le pasen
      el resultado de sus tiradas al proceso principal */
   if(crear_cm(&msqid_tiradas, N_KEY_CABALLOS)== -1){
      printf("Fallo en la creacion de la cola de mensajes.\n");
      exit(EXIT_FAILURE);
   }
   /* Creacion de la cola de mensajes para que el generador de apuestas
      le pase las apuestas al gestor */
   if(crear_cm(&msqid_apuestas, N_KEY_APUESTAS) == -1){
      printf("Fallo en la creacion de la cola de mensajes.\n");
      exit(EXIT_FAILURE);
   }

   /* No tengo ni zorra de lo que estoy haciendo */
   pid_caballos = (pid_t *) malloc(sizeof(pid_t) * n_caballos);
   posiciones = (int *) malloc(sizeof(int) * n_caballos);

   /* Creacion de los procesos */
   for(i = 0; i < n_caballos + 3; i++){
      if(i > 2){
         if(pipe(fd)==-1){
            printf("Error creando la tubería.\n");
            exit(EXIT_FAILURE);
         }
      }
      if((pid = fork()) == -1){
         printf("Error al hacer el fork.\n");
         exit(EXIT_FAILURE);
      } else if(!pid){
         if(i == 0){
            /* Proceso monitor */

            /* Mostrar el status de la carrera : comenzada? */
            /* Posicion de los caballos */
            /* Estados de las apuestas */

            /* Si la carrera ha acabdo : finalizada */
            /* Tres primeros puestos */
            /* Resultados de las apuestas */
         } else if(i == 1){
            /* Proceso gestor de apuestas */

            /* Una movida bastisima */
         } else if(i == 2){
            /* Proceso apostador */

            /* Cada 0,1 segundos genera una apuesta y envia el mensaje al gestor */
         } else{
            /* Caballitos */
            int max, min;
            int tirada;
            /* Hacemos un pause para que el proceso principal nos de la info de las posiciones y demas ? */
            if(pause() != -1){
               printf("No se en que momento da este error.\n");
               exit(EXIT_FAILURE);
            }
            /* Leemos los datos para decidir la tirada */
            close(fd[1]);
            read(fd[0], posiciones, sizeof(posiciones));
            max = 0;
            min = INT_MAX;
            for(j = 0; j < n_caballos; j++){
               if(max < posiciones[j]){
                  max = posiciones[j];
               }
               if(min > posiciones[j]){
                  min = posiciones[j];
               }
            }
            /* Una vez sabemos el que va en cabeza y el ultimo miramos a ver si somos nosotros 
               y tiramos */
            if(posiciones[i - 3] == max){
               tirada = tirada_ganadora();
            } else if(posiciones[i - 3] == min){
               tirada = tirada_remontadora();
            } else{
               tirada = tirada_normal();
            }

            mensaje.tipo = 1;
            mensaje.tirada = tirada;
            /* Mandamos nuestra info al proceso principal a traves de mensaje */
            if(enviar_m(msqid_tiradas, &mensaje) == -1){
               printf("Error al mandar el mensaje desde los caballos al proceso principal.\n");
               exit(EXIT_FAILURE);
            }

         }
         break;
      } else{
         /* Guardamos los pids de los caballos o que ? */
         if(i > 2){
            pid_caballos[i - 3] = pid;
         }
      }
   }

   if(i == n_caballos){
      /* Proceso principal */

      /* Inicializacion de las posiciones de cada caballo ? */
      for(j = 0; j < n_caballos; j++){
         posiciones[j] = 0;
      }
      /* Escribimos para cada caballo? No tiene sentido esto no ?  No me acuerdo de tuberias sorry */
      //for(j = 0; j < n_caballos; j++){
         close(fd[0]);
         write(fd[1], posiciones, sizeof(posiciones));
      //}

      /* Le mandamos una señal a cada caballo ?  Como ?  Array de pids guarro ? */
      for(j = 0; j < n_caballos; j++){
         if(enviar_senal(pid_caballos[j], SIGUSR1) == -1){
            printf("Fallo al enviar la señal desde el proceso principal a los caballos.\n");
            exit(EXIT_FAILURE);
         }
      }

      for(j == 0; j < n_caballos; j++){
         if(recibir_m(msqid_tiradas, &mensaje, 1) == -1){
            printf("Error al recibir la informacion sobre las tiradas de los caballos.\n");
            exit(EXIT_FAILURE);
         }
         posiciones[j] += mensaje.tirada;
      }
      /* Algun caballo ha llegado ya a la meta ? Este ejercicio no tiene sentido en eficiencia 
         (o lo estoy haciendo tremendamente mal) */
      for(j = 0; j < n_caballos; j++){
         if(posiciones[j] >= longitud){
            /* Pues terminamos */
            printf("%d", posiciones[j]);
            fflush(stdout);
            eliminar_cm(msqid_tiradas);
            eliminar_cm(msqid_apuestas);
         }
      }
   }



}


void manejador_SIGTERM(int sig){
   /* Liberar cosillas y demás */
}

void manejador_SIGUSR1(int sig){
   /* La reciben todos los caballitos y saben que el proceso padre les
      esta esperando */
}

int tirada_normal(){
   return rand() % 6 + 1;
}

int tirada_ganadora(){
   return rand() % 7 + 1;
}

int tirada_remontadora(){
   return tirada_normal() + tirada_normal();
}
