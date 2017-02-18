#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define NUM_PROC 3
#define MAX_CHAR 128

int main (void){
  pid_t pid;
  int i;
  char pid_c[MAX_CHAR];

  for(i = 0 ; i<NUM_PROC; i++) {
    if((pid=fork())<0) {
      printf("Error haciendo fork().\n");
      exit(EXIT_FAILURE);
    } else if (pid==0) {
      printf("HIJO %d, PID: %d, PPID: %d\n", i, getpid(), getppid());
    } else{
      printf("PADRE %d, PID: %d, PPID: %d\n", i, getpid(), getppid());

      if((pid=fork())<0) {
        exit(EXIT_FAILURE);
      } else if (pid==0) {
        sprintf(pid_c, "%d", getppid());
        char *buf[] = {"pstree", pid_c, "-p", NULL};
        //fprintf(stdout, "%s %s %s %d\n", buf[0], buf[1], buf[2], getpid());        
        if (execvp("pstree", buf) == -1) {
          printf("Entras aquí alguna vez?\n");
          exit(EXIT_FAILURE);
        }
      } else{
        waitpid(pid, NULL, 0);
      } 
    }
  }

  exit(EXIT_SUCCESS);
}