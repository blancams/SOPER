#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define NUM_PROC 4

int main (void){
  pid_t pid;
  int i;

  int fd1[2], fd2[2];
  char buf[150];
  int op1, op2;

  for(i=0; i<NUM_PROC; i++) {
    if(pipe(fd1)==-1){
      printf("Error creando la tubería.\n");
      exit(EXIT_FAILURE);
    }
    if(pipe(fd2)==-1){
      printf("Error creando la tubería.\n");
      exit(EXIT_FAILURE);
    }
    if((pid=fork())<0) {
      printf("Error haciendo fork().\n");
      exit(EXIT_FAILURE);
    } else if (pid==0) {
      close(fd1[1]);
      close(fd2[0]);
      read(fd1[0], buf, sizeof(buf));
      op1 = atoi(strtok(buf, ","));
      op2 = atoi(strtok(NULL,","));
      if(i==0){
        sprintf(buf, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Suma: %d\n", getpid(), op1, op2, op1+op2);
      } else if(i==1){
        sprintf(buf, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Resta: %d\n", getpid(), op1, op2, op1-op2);
      } else if(i==2){
        sprintf(buf, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. Multiplicación: %d\n", getpid(), op1, op2, op1*op2);
      } else{
        sprintf(buf, "Datos enviados a través de la tubería por el proceso PID=%d. Operando 1: %d. Operando 2: %d. División: %d\n", getpid(), op1, op2, op1/op2);
      }
      write(fd2[1], buf, strlen(buf)+1);
      exit(EXIT_SUCCESS);
    } else{
      close(fd1[0]);
      close(fd2[1]);
      write(fd1[1], "5,6 ", 4);
      read(fd2[0], buf, sizeof(buf));
      printf("%s", buf);
      waitpid(pid, NULL, 0);
    }
  }
  exit(EXIT_SUCCESS);
}
  