/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado la cabecera para la implementacion
 * de la libreria de trabajo con hilos.
 *
 * @file hilos.h
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#ifndef HILOS_H
#define HILOS_H

#define OK 2						/*!< Valor para correcto retorno de funciones */
#define ERROR -1					/*!< Valor para incorrecto retorno de funciones */

/**
 * @brief Crea un hilo asociado a la funcion indicada
 *
 * @param pthread_t *hilo: Puntero a pthread_t donde se devolvera el hilo creado.
 * @param void*(*funcion)(void*): Funcion asociada al hilo que se quiere crear.
 * @param void *args: Argumentos de entrada a la funcion asociada.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int crear_hilo(pthread_t *hilo, void*(*funcion)(void*), void *args);

/**
 * @brief Une un hilo a la ejecucion del programa
 *
 * @param pthread_t hilo: Hilo cuya ejecucion quiere respetarse.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int unir_hilo(pthread_t hilo);

/**
 * @brief Cancela la ejecucion de un hilo
 *
 * @param pthread_t hilo: Hilo cuya ejecucion quiere cancelarse.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int salir_hilo(pthread_t hilo);

#endif
