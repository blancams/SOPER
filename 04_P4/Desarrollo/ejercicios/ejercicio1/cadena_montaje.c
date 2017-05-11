/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado el codigo del primer
 * ejercicio de la cuarta practica, que consiste en la creacion de una
 * cadena de montaje a traves de procesos que se comunican por medio
 * de una cola de mensajes.
 *
 * @file cadena_montaje.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

#define NUM_PROC 3         /*!< Numero de procesos a crear */
#define MAX_CHAR 4000      /*!< Numero maximo de bytes para lectura */
#define N_KEY 300          /*!< Numero para la generacion de la clave de la cola de mensajes */
#define F_KEY "/bin/cat"   /*!< Fichero para la generacion de la clave de la cola de mensajes */

/**
 * @brief Estructura que se utiliza como contenido de los mensajes.
 *
 * Estructura que contiene el tipo de mensaje, una cadena de caracteres, el tamaño
 * de la cadena y un indicador de finalizacion.
 */
typedef struct _Mensaje{
   long tipo;
   char mtext[MAX_CHAR];
   int lastSize;
   int fin;
}mensaje;

/**
 * @brief Funcion main del ejercicio 1
 *
 * El programa consiste en la creacion de tres procesos que cooperaran en una cadena de montaje.
 * El primero (A) leera un maximo de 4KBytes de un fichero, e ira pasandole esa informacion
 * al segundo proceso (B), que transformara el texto a mayusculas y se lo enviara
 * al ultimo proceso (C) que escribira la informacion en un segundo fichero. 
 * La comunicacion entre procesos se realiza a traves de una cola de mensajes.
 *
 * @param int argc: Numero de argumentos de entrada.
 * @param char *argv[]: argv[1] hace referencia al fichero de entrada y argv[2] al fichero de salida.
 * @return EXIT_SUCCESS si se han realizado correctamente todas las tareas, EXIT_FAILURE si
 * se ha producido algun error al reservar recursos o al liberarlos.
 */
int main(int argc, char *argv[]){

	int pid;
	int i;
   key_t clave;
   int msqid;
   mensaje msg;

   /* Comprobacion del numero de argumentos de entrada */
   if(argc != 3){
      printf("Error en los argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   }

   /* Generacion de la clave y creacion de la cola de mensajes con ella */
   clave = ftok(F_KEY, N_KEY);
   if(clave == (key_t) -1){
      printf("Error al generar la clave.\n");
      exit(EXIT_FAILURE);
   }

   msqid = msgget(clave, IPC_CREAT | IPC_EXCL | 0660);
   if(msqid == -1){
      printf("Error al crear la cola de mensajes. %s\n", strerror(errno));
      exit(EXIT_FAILURE);
   }

   /* Creacion de los tres procesos */
   msg.fin = 0;
	for(i = 0; i < NUM_PROC; ++i){
      if((pid = fork()) == -1){
         printf("Error en el fork().\n");
         exit(EXIT_FAILURE);
      }
      else if(!pid){
         break;
      }
	}

   if(i == 0){
      /* Proceso A */
      /* Apertura de fichero */
      int fd = open(argv[1], O_RDONLY);
      if(fd < 0){
         printf("Error al abrir el fichero de lectura.\n");
         exit(EXIT_FAILURE);
      }
      /* Acceso a la cola de mensajes */
      msqid = msgget(clave, 0660);
      if(msqid == -1){
         printf("Error al acceder a la cola de mensajes. %s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      /* Lectura del fichero y envio de mensaje al proceso B */
      while((msg.lastSize = read(fd, msg.mtext, sizeof(msg.mtext))) > 0){
         msg.tipo = 1;
         sleep(1);
         if (msgsnd(msqid, &msg, sizeof(mensaje) - sizeof(long), 0) == -1){
            printf("%s\n", strerror(errno));
            exit(EXIT_FAILURE);
         }
      }
      /* Mensaje de finalizacion al proceso B */
      msg.tipo = 1;
      msg.fin = 1;
      sleep(1);
      if (msgsnd(msqid, &msg, sizeof(mensaje) - sizeof(long), 0) == -1){
         printf("%s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      /* Liberacion de recursos */
      close(fd);
   } else if(i == 1){
      /* Proceso B */
      int cnt;
      /* Acceso a la cola de mensajes */
      msqid = msgget(clave, 0660);
      if(msqid == -1){
         printf("Error al acceder a la cola de mensajes. %s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      while(1){
         /* Recepcion del mensaje y comprobacion de finalizacion */
         if(msgrcv(msqid, &msg, sizeof(mensaje) - sizeof(long), 1,  0) == -1){
            printf("%s\n", strerror(errno));
            exit(EXIT_FAILURE);
         }
         if(msg.fin == 1){
            break;
         }
         /* Conversion a mayusculas */
         cnt = 0;
         while(cnt < strlen(msg.mtext)){
            msg.mtext[cnt] = (char) toupper(msg.mtext[cnt]);
            cnt++;
         }
         /* Envio del mensaje en mayusculas al proceso C */
         msg.tipo = 2;
         if(msgsnd(msqid, &msg, sizeof(mensaje) - sizeof(long), 0) == -1){
            printf("%s\n", strerror(errno));
            exit(EXIT_FAILURE);
         }
      }
      /* Mensaje de finalizacion al proceso C */
      msg.tipo = 2;
      if (msgsnd(msqid, &msg, sizeof(mensaje) - sizeof(long), 0) == -1){
         printf("%s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
   } else if(i == 2){
      /* Proceso C */
      /* Apertura del fichero de salida */
      int fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);
      if(fd < 0){
         printf("Error al abrir el fichero de escritura.\n");
         exit(EXIT_FAILURE);
      }
      /* Acceso a la cola de mensajes */
      msqid = msgget(clave, 0660);
      if(msqid == -1){
         printf("Error al acceder a la cola de mensajes. %s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      while(1){
         /* Recepcion del mensaje y comprobacion de finalizacion */
         if(msgrcv(msqid, &msg, sizeof(mensaje) - sizeof(long), 2,  0) == -1){
            printf("%s\n", strerror(errno));
            exit(EXIT_FAILURE);
         }
         if(msg.fin == 1){
            break;
         }
         /* Escritura del texto en el fichero */
         write(fd, msg.mtext, msg.lastSize);
      }
      /* Liberacion de recursos */
      close(fd);
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
   } else{
      /* El proceso padre espera la finalizacion de los hijos */
      for(i = 0; i < NUM_PROC; i++){
         wait(NULL);
      }
   }
   exit(EXIT_SUCCESS);
}
