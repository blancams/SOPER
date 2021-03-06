/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado la cabecera para la implementacion
 * de la libreria del comportamiento de los procesos caballo.
 *
 * @file caballo.h
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#ifndef CABALLO_H
#define CABALLO_H

#define OK 2						/*!< Valor para correcto retorno de funciones */
#define ERROR -1					/*!< Valor para incorrecto retorno de funciones */

/**
 * @brief Estructura para la comunicacion de los caballos al proceso principal.
 *
 * Estructura que contiene el tipo de mensaje, junto con el resultado de la tirada.
 */
typedef struct _Caballos_Principal{
   long tipo;
   int tirada;
} caballo_principal;

/**
 * @brief Funcion que recoge el comportamiento de cada uno de los procesos caballo.
 *
 * @param int i: Identificador del caballo.
 * @param int fd: Descriptor de fichero para la comunicacion del proceso principal y los caballos.
 * @param int n_caballos: Numero de caballos.
 * @param int key: Parametro para acceso a cola de mensajes.
 * @return int: -1 si ha ocurrido algun error, 0 en caso de ejecucion normal.
 */
int caballo(int i, int fd, int n_caballos, int key);

/**
 * @brief Generador de un numero aleatorio entre 1 y 6.
 *
 * @return int: Entero aleatorio entre 1 y 6.
 */
int tirada_normal();

/**
 * @brief Generador de un numero aleatorio entre 1 y 7.
 *
 * @return int: Entero aleatorio entre 1 y 7.
 */
int tirada_ganadora();

/**
 * @brief Suma de dos numeros aleatorios entre 1 y 6.
 *
 * @return int: Entero entre 1 y 12.
 */
int tirada_remontadora();

#endif
