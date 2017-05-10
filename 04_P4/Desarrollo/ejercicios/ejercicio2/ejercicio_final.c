
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "../../senales/senales.h"
#include "../../memcomp/memcomp.h"
#include "../../mensajes/mensajes.h"
#include "../../hilos/hilos.h"
#include "../../semaforos/semaforos.h"

#define N_KEY_CABALLOS 300
#define N_KEY_APUESTAS 247
#define N_KEY_ACCGTAPU 275
#define N_KEY_SEMAFORO 72345
#define N_KEY_POSICION 334
#define MAX_APUESTA 500
#define MAX_CHAR 512

void manejador_SIGTERM(int sig);
void manejador_SIGUSR1(int sig);

int tirada_normal();
int tirada_ganadora();
int tirada_remontadora();

int caballos(int i, int *fd, int *posiciones, int n_caballos);
void *ventanilla(void *arg);
void imprimir_carrera(char *estado, int n_caballos, int *posiciones, double *cotizaciones);
void imprimir_finalizada(int n_caballos, int *posiciones, double *ganancia);

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

/* (Fer) Estructura para saber en todo momento los datos de las apuestas */
typedef struct _Apuestas_Total{
   double *apostado;
   double *cotizacion;
   double total;
   double *ganancia;
} apuestas_total;

/* (Fer) Estructura para mandar como argumento del gestor a los hilos ventanilla */
typedef struct _Gestor_Ventanilla{
   int msqid_apuestas;
   int semid;
   apuestas_total *apuestas;
} str_ventanilla;

int main(int argc, char *argv[]){

   int i, j, k;
   int n_caballos, longitud, n_apostadores, n_ventanillas;
   int pid;
   int msqid, shmid_apuestas, shmid_posiciones, semid;
   int fd[2];
   pid_t * pid_procesos;
   int *posiciones;
   unsigned short semval;
   caballo_principal mensaje;
   apostador_gestor ap_generada;
   apuestas_total apuestas;
   pthread_t *hilos;

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

   /* No tengo ni zorra de lo que estoy haciendo */
   /* (Fer) Pues bastante idea tenías. He añadido todas las cosas extra que requieren
   reserva de memoria: los hilos y los arrays con datos de apuestas. Faltan. */
   pid_procesos = (pid_t *) malloc(sizeof(pid_t) * (n_caballos+3));
   posiciones = (int *) malloc(sizeof(int) * n_caballos);
   hilos = (pthread_t *) malloc(sizeof(pthread_t) * n_ventanillas);

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

   /* (Fer) Para inicializar la semilla de numeros aleatorios */
   srand(time(NULL));

   /* Creacion de la cola de mensajes para que los caballos le pasen
      el resultado de sus tiradas al proceso principal */
   if(crear_cm(&msqid, N_KEY_CABALLOS)== -1){
      printf("Fallo en la creacion de la cola de mensajes 1.\n");
      exit(EXIT_FAILURE);
   }
   /*
    Creacion de la cola de mensajes para que el generador de apuestas
      le pase las apuestas al gestor
   if(crear_cm(&msqid_apuestas, N_KEY_APUESTAS) == -1){
      printf("Fallo en la creacion de la cola de mensajes 2.\n");
      exit(EXIT_FAILURE);
   }
   */
      /* (Fer) Creacion de la memoria compartida para que gestor de apuestas y
         monitor tengan la informacion de las apuestas */
   if(crear_shm(sizeof(apuestas_total), &shmid_apuestas, N_KEY_ACCGTAPU) == -1) {
      printf("Fallo en la creacion de memoria compartida.\n");
      exit(EXIT_FAILURE);
   }

   if(crear_shm(sizeof(posiciones), &shmid_posiciones, N_KEY_POSICION) == -1) {
      printf("Fallo en la creacion de memoria compartida.\n");
      exit(EXIT_FAILURE);
   }

   /* (Fer) Creacion del array de semaforos (de momento con uno solo, puede que
      acabe habiendo mas). */
   if (Crear_Semaforo(N_KEY_SEMAFORO, 1, &semid) == -1) {
      printf("Error al crear los semáforos.\n");
      exit(EXIT_FAILURE);
   }

   /* Inicializacion del semaforo */
   semval = 1;
   if (Inicializar_Semaforo(semid, &semval) == -1) {
      printf("Error al inicializar los semáforos.\n");
      exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
         } else {
            printf("Lo hace.\n");
            printf("%d %d\n", fd[0], fd[1]);
         }
      }

      if((pid = fork()) == -1) {

         printf("Error al hacer el fork.\n");
         exit(EXIT_FAILURE);

      } else if(!pid){

         if (i > 2) {
            close(fd[1]);
         }

         break;

      } else {
         /* Guardamos los pids de los caballos o que ? */
         /* (Fer) Y tambien los de los otros procesos :D */
         pid_procesos[i] = pid;

      }

   }

   /* (Fer) Aqui no he tocado nada */
   if(i == 0){
      /* Proceso monitor */
      /* Debera tener acceso a todos los mensajes, las areas de memoria compartida, etc */
      char estado[MAX_CHAR];

      /* (Fer) Acceso a cola de mensajes */
      if (acceder_shm(shmid_apuestas, (char*) &apuestas) == -1) {
         printf("Error al acceder a memoria compartida en gestor.\n");
         exit(EXIT_FAILURE);
      }

      if (acceder_shm(shmid_posiciones, (char*) posiciones) == -1) {
         printf("Error al acceder a memoria compartida en gestor.\n");
         exit(EXIT_FAILURE);
      }

      for (j = 0; j < 15; j++) {
         sprintf(estado, "Estado de la carrera: faltan %d segundos.", 15-j);
         imprimir_carrera(estado, n_caballos, posiciones, apuestas.cotizacion);
         usleep(1000);
      }

      sprintf(estado, "Estado de la carrera: comenzada.");

      while(1) {
         imprimir_carrera(estado, n_caballos, posiciones, apuestas.cotizacion);
      }

      imprimir_finalizada(n_caballos, posiciones, apuestas.ganancia);

      exit(EXIT_SUCCESS);
      /* Mostrar el status de la carrera : comenzada? */
      /* Posicion de los caballos */
      /* Estados de las apuestas */

      /* Si la carrera ha acabdo : finalizada */
      /* Tres primeros puestos */
      /* Resultados de las apuestas */
   } else if(i == 1){
      /* (Fer) Aqui hay cosas. Desde este proceso (el gestor de apuestas) accedo a la
      memoria compartida para compartir info con el monitor; accedo a la cola de
      mensajes con la que nos comunicamos con el generador de apuestas; creo la
      estructura para comunicarme con los hilos ventanilla; inicializo la estructura
      de datos de apuestas; creo los hilos ventanilla; pauso el proceso principal;
      cuando reciba una señal, cancelo todos los hilos, los espero y se termina el
      proceso principal. */
      str_ventanilla str;

      apuestas.apostado = (double *) malloc(sizeof(double) * n_caballos);
      apuestas.ganancia = (double *) malloc(sizeof(double) * n_apostadores);
      apuestas.cotizacion = (double *) malloc(sizeof(double) * n_caballos);

      /* (Fer) Acceso a memoria compartida */
      if (acceder_shm(shmid_apuestas, (char*) &apuestas) == -1) {
         printf("Error al acceder a memoria compartida en gestor.\n");
         exit(EXIT_FAILURE);
      }

      /* (Fer) Acceso a cola de mensajes */
      if(crear_cm(&msqid, N_KEY_CABALLOS) == -1){
         printf("Fallo en el acceso a la cola de mensajes 2.\n");
         exit(EXIT_FAILURE);
      }

      /* (Fer) Estructura de argumento a ventanillas */
      str.msqid_apuestas = msqid;
      str.semid = semid;
      str.apuestas = &apuestas;

      /* (Fer) Inicializacion info apuestas */
      for (j = 0, k = 0; j < n_caballos || k < n_apostadores; j++, k++) {
         apuestas.apostado[j] = 1.0;
         apuestas.ganancia[k] = 0.0;
      }
      apuestas.total = 1.0 * n_caballos;

      /* (Fer) Creacion hilos */
      for (j = 0; j < n_ventanillas; j++) {
         if (crear_hilo(&hilos[j], ventanilla, (void*) &str) == -1) {
            printf("Error al crear hilos.\n");
            exit(EXIT_FAILURE);
         }
      }

      /* (Fer) Pausa */
      if(pause() != -1){
         printf("Fallo en pause de gestor.\n");
         return -1;
      }

      /* (Fer) Cancelacion y union con hilos */
      for (j = 0; j < n_ventanillas; j++) {
         if (salir_hilo(hilos[j]) == -1) {
            printf("Error al terminar hilos.\n");
            exit(EXIT_FAILURE);
         }

         if (unir_hilo(hilos[j]) == -1) {
            printf("Error al esperar hilos.\n");
            exit(EXIT_FAILURE);
         }
      }

      /* Liberacion de recursos */
      if(salir_shm((char*) &apuestas) == -1){
         printf("Error al desvincularse de la memoria compartida.\n");
         exit(EXIT_FAILURE);
      }
      free(apuestas.apostado);
      free(apuestas.cotizacion);
      free(apuestas.ganancia);

      exit(EXIT_SUCCESS);
      /* Una movida bastisima */
      /* (Fer) Correcto */
   } else if(i == 2){
      /* (Fer) Aqui no he hecho nada aun tampoco */
      char nom_apostador[13];
      /* Proceso apostador */

      /* (Fer) Acceso a cola de mensajes */
      if(crear_cm(&msqid, N_KEY_APUESTAS) == -1){
         printf("Fallo en el acceso a la cola de mensajes 2.\n");
         exit(EXIT_FAILURE);
      }

      while(1) {
         usleep(100);

         ap_generada.tipo = 2;
         sprintf(nom_apostador, "Apostador-%d", rand() % n_apostadores + 1);
         strcpy(ap_generada.nombre, nom_apostador);
         ap_generada.caballo = rand() % n_caballos;
         ap_generada.apuesta = ((double)rand() / (double)RAND_MAX) * MAX_APUESTA;

         if (enviar_m(msqid, &ap_generada) == -1) {
            printf("Error al enviar mensaje desde generador.\n");
            exit(EXIT_FAILURE);
         }
      }

      /* Cada 0,1 segundos genera una apuesta y envia el mensaje al gestor */
   } else if(i == n_caballos + 3){
      /* Proceso principal */
      /* (Fer) Aqui he: cerrado la tuberia y creado un bucle para la simulacion, donde
         basicamente he copiado lo que tu ya tenias, cambiando algunas cosas para adaptar
         los cambios hechos en el resto del codigo. Hay que arreglar la parte de la
         liberacion de recursos pero meh, al final. */
      close(fd[0]);

      if (acceder_shm(shmid_posiciones, (char*) posiciones) == -1) {
         printf("Error al acceder a memoria compartida en gestor.\n");
         exit(EXIT_FAILURE);
      }

      /* Inicializacion de las posiciones de cada caballo ? */
      for(j = 0; j < n_caballos; j++){
         posiciones[j] = 0;
      }

      while(1) {
         /* Escribimos para cada caballo? No tiene sentido esto no ?  No me acuerdo de tuberias sorry */
         write(fd[1], posiciones, sizeof(posiciones));

         /* Le mandamos una señal a cada caballo ?  Como ?  Array de pids guarro ? */
         for(j = 3; j < n_caballos+3; j++){
            if(enviar_senal(pid_procesos[j], SIGUSR1) == -1){
               printf("Fallo al enviar la señal desde el proceso principal a los caballos.\n");
               exit(EXIT_FAILURE);
            }
         }
         for(j = 0; j < n_caballos; j++){
            if(recibir_m(msqid, &mensaje, 1) == -1){
               printf("Error al recibir la informacion sobre las tiradas de los caballos.\n");
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
      }

      printf("%d", posiciones[j]);
      fflush(stdout);

      /* Liberacion de recursos */
      if(eliminar_shm(shmid_apuestas) == -1){
         printf("Error al eliminar la region de memoria compartida.\n");
         exit(EXIT_FAILURE);
      }
      if(Borrar_Semaforo(semid) == -1){
         printf("Error al eliminar el semaforo.\n");
         exit(EXIT_FAILURE);
      }
      if(eliminar_cm(msqid) == -1){
         printf("Error al eliminar la cola de mensajes.\n");
         exit(EXIT_FAILURE);
      }
      free(pid_procesos); 
      free(posiciones);
      free(hilos);

   } else {
      /* (Fer) Esto son ya todos los procesos de los caballos. Lanzan la funcion caballos,
      mas abajo */
      if (caballos(i, fd, posiciones, n_caballos) == -1){
         printf("Fallo en caballos %d.\n", i);
         exit(EXIT_FAILURE);
      }
   }

}

/* (Fer) Esto esta en proceso porque aun no se muy bien como vamos a hacer para avisar
a todos los procesos de que se acaba la carrera. Estoy pensando en que, como CTRL+C
es SIGINT, podemos hacer un manejador de SIGINT que mande SIGTERM a todos los procesos,
donde el manejador de SIGTERM haga lo de poner una variable a 1 cuando al principio
estaba en 0, y que esto sea la condicion de salida de los bucles */
void manejador_SIGTERM(int sig){
   // (Blanca) terminar deberia ser una variable global si quieres hacerlo asi
   // pero como los caballos no reservan ningun tipo de memoria ni nada,
   // creo que podriamos poner exit(EXIT_SUCCESS) no?
  /*
   terminar = 1;
   return;
   */
   exit(EXIT_SUCCESS);
}

void manejador_SIGUSR1(int sig){
   /* La reciben todos los caballitos y saben que el proceso padre les
      esta esperando */
   return;
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

/* (Fer) Amo a vé. Creo que esta clarinete: accedo a la cola de mensajes para enviarlos
al proceso principal y comienzo el bucle, donde esta lo que habias hecho ya tu con
algunas correcciones. Cuando sale del bucle (terminar=1) se retorna guay, si no con
error */
int caballos(int i, int *fd, int *posiciones, int n_caballos) {
   /* Caballitos */

   int max, min, j, tirada, msqid, terminar;
   caballo_principal mensaje;
   int caballos;

   /* (Fer) Valor para la finalización controlada del bucle */
   terminar = 0;

   /* (Fer) Acceso a la cola de mensajes de los caballos */
   if(crear_cm(&msqid, N_KEY_CABALLOS)== -1){
      printf("Fallo en la creacion de la cola de mensajes (caballos).\n");
      return -1;
   }

   /* (Fer) Bucle principal de los caballos */
   while(!terminar) {
      /* Hacemos un pause para que el proceso principal nos de la info de las posiciones y demas ? */
      if(pause() != -1){
         printf("No se en que momento da este error.\n");
         return -1;
      }

      /* Leemos los datos para decidir la tirada */
      read(fd[0], posiciones, sizeof(posiciones));
      max = 0;
      min = INT_MAX;

      /* Cálculo del caballo líder y último */
      caballos = n_caballos;
      for(j = 0; j < caballos; j++){
         if(max < posiciones[j]){
            max = posiciones[j];
         }
         if(min > posiciones[j]){
            min = posiciones[j];
         }
      }

      /* Una vez sabemos el que va en cabeza y el ultimo miramos a ver si somos nosotros
         y tiramos */
      if (max) {
         if(posiciones[i - 3] == max){
            tirada = tirada_ganadora();
         } else if(posiciones[i - 3] == min){
            tirada = tirada_remontadora();
         } else{
            tirada = tirada_normal();
         }
      } else {
         tirada = tirada_normal();
      }

      /* Definicion de la estructura a enviar por mensaje */
      mensaje.tipo = 1;
      mensaje.tirada = tirada;

      /* Mandamos nuestra info al proceso principal a traves de mensaje */
      if(enviar_m(msqid, &mensaje) == -1){
         printf("Error al mandar el mensaje desde los caballos al proceso principal.\n");
         return -1;
      }

   }

   /* (Fer) Cuando se sale del bucle, retornamos correctamente */
   return 0;

}

/* (Fer) Esta es chunguita. Basicamente es lo que ejecuta cada hilo del gestor de apuestas.
Recibo como argumento un puntero a la estructura donde tengo el identificador de la
cola de mensajes de las apuestas generadas y la estructura con la info general de
apuestas, y hago los cambios pertinentes. No he implementado nada de los semaforos en
todo el codigo, lo cual evidentemente falta (crearlo y tal), pero creo que es necesario
que no escriban varias ventanillas a la vez, porque si no se puede joder el calculo de
algunas cosas. De hecho, en la funcion de los caballos estoy muy rayado porque tengo
la sensacion de que hace falta algun tipo de sincronizacion, aunque como al fin y al
cabo la comunicacion va por tuberias y mensajes igual no hace falta nada mas... veremos */
void *ventanilla(void *arg) {
   str_ventanilla *str = (str_ventanilla*) arg;
   int apostador, caballo;
   double cantidad;
   apostador_gestor apuesta;

   while(1) {
      if(recibir_m(str->msqid_apuestas, &apuesta, 2) == -1){
         printf("Error al recibir la informacion sobre las tiradas de los caballos.\n");
         exit(EXIT_FAILURE);
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
         exit(EXIT_FAILURE);
         // (BLanca)  Tenias puesto return ERROR;
      }

      str->apuestas->total += cantidad;
      str->apuestas->apostado[caballo] += cantidad;
      str->apuestas->cotizacion[caballo] = str->apuestas->total / str->apuestas->apostado[caballo];
      str->apuestas->ganancia[apostador] = cantidad * str->apuestas->cotizacion[caballo];

      if (Up_Semaforo(str->semid, 0, 0) == -1) {
         printf("Error al ejecutar función Up_Semaforo.\n");
         exit(EXIT_FAILURE);
         // (BLanca)  Tenias puesto return ERROR;
      }
   }
}

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
