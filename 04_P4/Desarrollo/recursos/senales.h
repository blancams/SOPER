/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado la cabecera para la implementacion
 * de la libreria de señales.
 *
 * @file semaforos.h
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#ifndef SENALES_H
#define SENALES_H

#define OK 2						/*!< Valor para correcto retorno de funciones */
#define ERROR -1					/*!< Valor para incorrecto retorno de funciones */

/**
 * @brief Envia una señal a un proceso.
 *
 * @param pid_t proceso: Identificador del proceso al que se quiere enviar la señal.
 * @param int senal: Señal que se quiere enviar.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int enviar_senal(pid_t proceso, int senal);

/**
 * @brief Crea la mascara de señales especificada para una señal.
 *
 * @param sigset_t *mascara: Puntero donde se guardara la mascara actual.
 * @param int senal: Señal a añadir a la mascara.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int crear_mascara(sigset_t *mascara, int senal);

/**
 * @brief Añade la señal a la mascara.
 *
 * @param int senal: Señal a añadir a la mascara.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int anadir_mascara(int senal);

/**
 * @brief Elimina la señal de la mascara.
 *
 * @param int senal: Señal a eliminar de la mascara.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int quitar_mascara(int senal);

/**
 * @brief Indica si se ha recibido alguna señal bloqueada.
 *
 * @param int senal: Señal a comprobar su recepcion.
 * @param int *value: Puntero donde se guardara un 0 si la señal no ha sido recibida, 1 del contrario.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int senal_bloqueada(int senal, int *value);

/**
 * @brief Asocia la recepcion de una señal a una rutina concreta.
 *
 * @param int senal: Señal que se recibe.
 * @param void *funcion: Nuevo manejador de la señal especificada.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int crear_manej(int senal, void *funcion);

#endif
