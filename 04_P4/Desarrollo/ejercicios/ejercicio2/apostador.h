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
#define NOM_CHAR 20					/*!< Maximo numero de caracteres */

/**
 * @brief Estructura que recoge los datos de una apuesta.
 *
 * Estructura que contiene el nombre del apostador,
 * el identificador del caballo y la apuesta realizada.
 */
typedef struct _Info_AG{
   char nombre[NOM_CHAR];
   int caballo;
   double apuesta;
} info_ag;

/**
 * @brief Estructura para la comunicacion del generador de apuestas al gestor.
 *
 * Estructura que contiene el tipo de mensaje, junto con una estructura que 
 * hace referencia a la apuesta.
 */
typedef struct _Apostador_Gestor{
   long tipo;
   info_ag ap;
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
