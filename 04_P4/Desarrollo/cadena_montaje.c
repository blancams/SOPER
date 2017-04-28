
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

#define NUM_PROC 3
#define MAX_CHAR 4000

#define N_KEY 300
#define F_KEY "/bin/cat"

typedef struct _Mensaje{
   long tipo;
   char mtext[MAX_CHAR];
   int lastSize;
}mensaje;	

int main(int argc, char *argv[]){

	int pid;
	int i;
   key_t clave;
   int msqid;
   mensaje msg;

   if(argc != 3){
      printf("Error en los argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   }

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
      int fd = open(argv[1], O_RDONLY);
      if(fd < 0){
         printf("Error al abrir el fichero de lectura.\n");
         exit(EXIT_FAILURE);
      }
      msqid = msgget(clave, 0660);
      if(msqid == -1){
         printf("Error al acceder a la cola de mensajes. %s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      while((msg.lastSize = read(fd, msg.mtext, sizeof(msg.mtext))) > 0){
         msg.tipo = 1;
         sleep(1);
         if (msgsnd(msqid, &msg, sizeof(mensaje) - sizeof(long), 0) == -1){
            printf("%s\n", strerror(errno));
            exit(EXIT_FAILURE);
         }
      }
      msg.tipo = 1;
      strcpy(msg.mtext, "FIN");
      sleep(1);
      if (msgsnd(msqid, &msg, sizeof(mensaje) - sizeof(long), 0) == -1){
         printf("%s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      close(fd);
   } else if(i == 1){
      int cnt;
      msqid = msgget(clave, 0660);
      if(msqid == -1){
         printf("Error al acceder a la cola de mensajes. %s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      while(1){
         if(msgrcv(msqid, &msg, sizeof(mensaje) - sizeof(long), 1,  0) == -1){
            printf("%s\n", strerror(errno));
            exit(EXIT_FAILURE);      
         }
         if(!strcmp(msg.mtext, "FIN")){
            break;
         } 
         cnt = 0;
         while(cnt < strlen(msg.mtext)){
            msg.mtext[cnt] = (char) toupper(msg.mtext[cnt]);
            cnt++;
         }
         msg.tipo = 2;
         if(msgsnd(msqid, &msg, sizeof(mensaje) - sizeof(long), 0) == -1){
            printf("%s\n", strerror(errno));
            exit(EXIT_FAILURE); 
         }
      }
      msg.tipo = 2;
      if (msgsnd(msqid, &msg, sizeof(mensaje) - sizeof(long), 0) == -1){
         printf("%s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
   } else if(i == 2){
      int fd = open(argv[2], O_WRONLY);
      if(fd < 0){
         printf("Error al abrir el fichero de escritura.\n");
         exit(EXIT_FAILURE);
      }
      msqid = msgget(clave, 0660);
      if(msqid == -1){
         printf("Error al acceder a la cola de mensajes. %s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      while(1){
         if(msgrcv(msqid, &msg, sizeof(mensaje) - sizeof(long), 2,  0) == -1){
            printf("%s\n", strerror(errno));
            exit(EXIT_FAILURE); 
         }
         if(!strcmp(msg.mtext, "FIN")){
            break;
         }
         write(fd, msg.mtext, msg.lastSize);
      }
      close(fd);
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
   } else{
      for(i = 0; i < NUM_PROC; i++){
         wait(NULL);
      }
   }
   exit(EXIT_SUCCESS);
}