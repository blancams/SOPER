#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "senales.h"

int enviar_senal(pid_t proceso, int senal) {
   int ret;

   ret = kill(proceso, senal);

   if (ret == 0) {
      return OK;
   } else {
      return ERROR;
   }
}

int crear_manej(int senal, void *funcion) {
   if (signal(senal, funcion) == SIG_ERR) {
      return ERROR;
   } else {
      return OK;
   }
}
