#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
   pid_t pid;
   int kid;

   if ((pid = fork()) == -1) {
      printf("Error al ejecutar fork.\n");
      exit(EXIT_FAILURE);
   } else if (pid == 0) {
      while(1) {
         printf("Soy el proceso hijo con PID: %d\n", getpid());
         sleep(5);
      }
   } else {
      sleep(30);
      kid = kill(pid, 15);
      if (kid == -1) {
         printf("Error al enviar señal.\n");
         exit(EXIT_FAILURE);
      }
   }

   wait(NULL);
   return EXIT_SUCCESS;
}
