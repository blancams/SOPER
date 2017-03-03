#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define NUM_PROC 3

int main (void){
   pid_t pid;
   int i;

   for(i=0 ; i<NUM_PROC; i++) {
      if((pid=fork())<0) {
         printf("Error haciendo fork().\n");
         exit(EXIT_FAILURE);
      } else if (pid==0) {
         printf("HIJO %d, PID: %d, PPID: %d\n", i, getpid(), getppid());
      } else{
         printf("PADRE %d, PID: %d, PPID: %d\n", i, getpid(), getppid());
         break;
      }
   }

   wait(NULL);
   exit(EXIT_SUCCESS);
}
