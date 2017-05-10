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

#define OK 2
#define ERROR -1

/**
 * @brief Envia una señal a un proceso.
 *
 * @param pid_t proceso: Identificador del proceso al que se quiere enviar la señal.
 * @param int senal: Señal que se quiere enviar.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int enviar_senal(pid_t proceso, int senal);

/**
 * @brief Asocia la recepcion de una señal a una rutina concreta.
 *
 * @param int senal: Señal que se recibe.
 * @param void *funcion: Nuevo manejador de la señal especificada.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int crear_manej(int senal, void *funcion);

#endif
