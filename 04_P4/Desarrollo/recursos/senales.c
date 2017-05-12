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
   if (kill(proceso, senal)) {
      return ERROR;
   } else {
      return OK;
   }
}

int crear_mascara(sigset_t *mascara, int senal) {
   if (sigemptyset(mascara)) {
      return ERROR;
   } else if (sigaddset(mascara, senal)) {
      return ERROR;
   } else if (sigprocmask(SIG_SETMASK, mascara, NULL)) {
      return ERROR;
   } else {
      return OK;
   }
}

int anadir_mascara(int senal) {
   sigset_t sset;

   if (sigaddset(&sset, senal)) {
      return ERROR;
   } else if (sigprocmask(SIG_BLOCK, &sset, NULL)) {
      return ERROR;
   } else {
      return OK;
   }
}

int quitar_mascara(int senal) {
   sigset_t sset;

   if (sigaddset(&sset, senal)) {
      return ERROR;
   } else if (sigprocmask(SIG_UNBLOCK, &sset, NULL)) {
      return ERROR;
   } else {
      return OK;
   }
}

int senal_bloqueada(int senal, int *value) {
   int ret;
   sigset_t bset;

   if (sigpending(&bset)) {
      return ERROR;
   } else {
      if ((ret = sigismember(&bset, senal)) == -1) {
         return ERROR;
      } else {
         *value = ret;
         return OK;
      }
   }
}

int crear_manej(int senal, void *funcion) {
   if (signal(senal, funcion) == SIG_ERR) {
      return ERROR;
   } else {
      return OK;
   }
}
