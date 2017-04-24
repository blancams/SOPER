
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#include <ctype.h>

#define NUM_PROC 3
#define MAX_CHAR 80	

#define N_KEY 300
#define F_KEY "/bin/cat"

typedef struct _Mensaje{
   long tipo;
   char mtext[MAX_CHAR];
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

   msqid = msgget(clave, IPC_CREAT | IPC_EXCL | 0660/*| S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH*/);
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

   if(i == 1){
      FILE *f1 = fopen(argv[1], "r+");
      if(!f1){
         printf("Error al abrir el fichero de lectura.\n");
         exit(EXIT_FAILURE);
      }
      fread(msg.mtext, sizeof(char), MAX_CHAR, f1);
      msg.tipo = 1;
      msqid = msgget(clave, 0660);
      if(msqid == -1){
         printf("Error al acceder a la cola de mensajes. %s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      if (msgsnd(msqid, &msg, sizeof(char)*strlen(msg.mtext), IPC_NOWAIT) == -1){
         printf("%s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      fclose(f1);
   } else if(i == 2){
      int cnt;
      msqid = msgget(clave, 0660);
      if(msqid == -1){
         printf("Error al acceder a la cola de mensajes. %s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      if(msgrcv(msqid, &msg, sizeof(mensaje) - sizeof(long), 1,  MSG_NOERROR) == -1){
         printf("%s\n", strerror(errno));
         exit(EXIT_FAILURE);      
      }
      while(cnt < strlen(msg.mtext)){
         msg.mtext[cnt] = (char) toupper(msg.mtext[cnt]);
         cnt++;
      }
      msg.tipo = 2;
      if(msgsnd(msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), IPC_NOWAIT) == -1){
         printf("%s\n", strerror(errno));
         exit(EXIT_FAILURE); 
      }
   } else if(i == 3){
      FILE *f2 = fopen(argv[2], "w+");
      if(!f2){
         printf("Error al abrir el fichero de escritura.\n");
         exit(EXIT_FAILURE);
      }
      msqid = msgget(clave, 0660);
      if(msqid == -1){
         printf("Error al acceder a la cola de mensajes. %s\n", strerror(errno));
         exit(EXIT_FAILURE);
      }
      if(msgrcv(msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), 2,  MSG_NOERROR) == -1){
         printf("%s\n", strerror(errno));
         exit(EXIT_FAILURE); 
      }
      fwrite(msg.mtext, sizeof(char), MAX_CHAR, f2);
      fclose(f2);
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
   } else{
      for(i = 0; i < NUM_PROC; i++){
         wait(NULL);
      }
   }
   exit(EXIT_SUCCESS);
}