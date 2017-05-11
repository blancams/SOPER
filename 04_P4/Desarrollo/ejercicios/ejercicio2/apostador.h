#ifndef APOSTADOR_H
#define APOSTADOR_H

#define OK 2
#define ERROR -1

/**
 * @brief Estructura para la comunicacion del generador de apuestas al gestor.
 *
 * Estructura que contiene el tipo de mensaje, junto con el nombre del apostador,
 * el identificador del caballo y la apuesta realizada.
 */
typedef struct _Apostador_Gestor{
   long tipo;
   char nombre[20];
   int caballo;
   double apuesta;
} apostador_gestor;

int apostador(int key, int n_apostadores, int n_caballos);

#endif
