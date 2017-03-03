#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define STRING_L 80

int main (void){
   pid_t pid;
   char *string;

   string = (char*) malloc (STRING_L*sizeof(char));

   if ((pid=fork()) < 0) {
      printf("Error haciendo fork().\n");
      exit(EXIT_FAILURE);
   } else if (pid == 0) {
      fprintf(stdout, "HIJO %d\n", getpid());
      fprintf(stdout, "Introduce un nombre: ");
      fscanf(stdin, "%s", string);
   } else {
      fprintf(stdout, "PADRE %d\n", getpid());
   }

   wait(NULL);
   fprintf(stdout, "El nombre introducido para %d es %s\n", getpid(), string);
   free(string);

   exit(EXIT_SUCCESS);
}
