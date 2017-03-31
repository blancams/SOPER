#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_CHAR 80
#define FILEKEY "/bin/cat"
#define KEY 1301

struct info {
   char nombre[MAX_CHAR];
   int id;
};

void manejador(int sig);
int num_aleatorio(int inf, int sup);

int main(int argc, char *argv[]){
   int key, id;
   int i, n;
   int pid;
   struct info *mens;

   if(argc != 2){
      printf("Fallo en los argumentos de entrada.\n");
      exit(EXIT_FAILURE);
   }
   n = atoi(argv[1]);

   key = ftok(FILEKEY, KEY);
   if(key == -1){
      printf("Error al generar la clave.\n");
      exit(EXIT_FAILURE);
   }

   id = shmget(key, sizeof(mens), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
   if(id == -1){
      printf("Error en la reserva del bloque de memoria.\n");
      exit(EXIT_FAILURE);
   }

   if(signal(SIGUSR1, manejador) == SIG_ERR){
      printf("Error en la asignacion del manejador.\n");
      exit(EXIT_FAILURE);
   }

   for(i = 0; i < n; i++){
      if((pid = fork()) == -1){
         printf("Error al hacer el fork.\n");
         exit(EXIT_FAILURE);
      } else if(!pid){
         break;
      }
   }

   if((mens = (struct info *) shmat(id, NULL, 0)) == NULL){
      printf("Error en el acceso a la memoria compartida.\n");
      shmdt((char *) mens);
      exit(EXIT_FAILURE);
   }

   if(pid){
      mens->id = 0;
      for(i = 0; i < n; i++){
         if(pause() != -1){
            printf("Error al hacer el pause.\n");
            exit(EXIT_FAILURE);
         }
         printf("Nombre de usuario: %s\tID: %d\n", mens->nombre, mens->id);
         fflush(stdout);
      }
      for(i = 0; i < n; i++){
         wait(NULL);
      }
      shmdt((char *) mens);
      shmctl(id, IPC_RMID, (struct shmid_ds *) NULL);
   } else if(!pid){
      srand(pid);
      sleep(num_aleatorio(i,10));
      printf("\nIntroduzca el nombre de un cliente: \n");
      scanf("%s", mens->nombre);
      mens->id++;
      shmdt((char *) mens);
      if(kill(getppid(), SIGUSR1) == -1){
         printf("Error en el envio de señal SIGUSR1.\n");
         exit(EXIT_FAILURE);
      }

   }
   exit(EXIT_SUCCESS);
}

void manejador(int sig){
   return;
}

int num_aleatorio(int inf, int sup){
   return rand() % (sup - inf + 1) + inf;
}
