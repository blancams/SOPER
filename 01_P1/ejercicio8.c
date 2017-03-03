#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define MAX_CHAR 64
#define LENGTH_INS 2

int main (int argc, char** argv) {
    int i;
    pid_t pid;
    char **argp = malloc(LENGTH_INS * sizeof(char*));
    char path[MAX_CHAR];

    for (i = 0; i < LENGTH_INS; i++) {
      argp[i] = (char*) malloc(MAX_CHAR);
   }

    if (argc < 2) {
      fprintf(stdout, "No hay argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   } else if (strcmp(argv[argc-1], "-l")*strcmp(argv[argc-1], "-lp")*strcmp(argv[argc-1], "-v")*strcmp(argv[argc-1], "-vp")) {
      fprintf(stdout, "Error al introducir el parámetro para la función exe (-l, -lp, -v, -vp).\n");
      exit(EXIT_FAILURE);
   }

   for (i = 1; i < (argc-1); i++) {
      printf("\n");
      if ((pid = fork()) < 0) {
         fprintf(stdout, "Error haciendo fork().\n");
         exit(EXIT_FAILURE);
      } else if (pid == 0) {
         if (!strcmp(argv[argc-1], "-l")) {
            sprintf(path, "/bin/%s", argv[i]);
            if (execl(path, argv[i], NULL)) {
               sprintf(path, "/usr/bin/%s", argv[i]);
               execl(path, argv[i], NULL);
            }
         } else if (!strcmp(argv[argc-1], "-lp")) {
            execlp(argv[i], argv[i], NULL);
         } else if (!strcmp(argv[argc-1], "-v")) {
            strcpy(argp[0], argv[i]);
            argp[1] = NULL;
            sprintf(path, "/bin/%s", argv[i]);
            if (execl(path, argv[i], NULL)) {
               sprintf(path, "/usr/bin/%s", argv[i]);
               execl(path, argv[i], NULL);
            }
            execv(path, argp);
         } else {
            strcpy(argp[0], argv[i]);
            argp[1] = NULL;
            execvp(argv[i], argp);
         }

         printf("Si este mensaje aparece se debe a que o bien se ha introducido ");
         printf("una instruccion incorrecta, o el ejecutable de dicha instruccion no ");
         printf("se encuentra ni en /usr/bin ni en /bin.\n");

      } else {
         wait(NULL);
      }
   }

   for (i = 0; i < LENGTH_INS; i++) {
      free(argp[i]);
   }
   free(argp);
   
   exit(EXIT_SUCCESS);

}
