/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado la cabecera para la implementacion
 * de la libreria del comportamiento del proceso apostador.
 *
 * @file apostador.h
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#ifndef APOSTADOR_H
#define APOSTADOR_H

#define OK 2						/*!< Valor para correcto retorno de funciones */
#define ERROR -1					/*!< Valor para incorrecto retorno de funciones */

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

/**
 * @brief Funcion que modela el comportamiento principal del generador de apuestas.
 *
 * @param int key: Clave para el acceso a la cola de mensajes.
 * @param int n_apostadores: Numero de apostadores.
 * @param int n_caballos: Numero de caballos.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int apostador(int key, int n_apostadores, int n_caballos);

#endif
