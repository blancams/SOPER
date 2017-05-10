/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * Este modulo consiste en la creacion
 * de una libreria que simplifique el uso de señales.
 *
 * @file senales.c
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
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
