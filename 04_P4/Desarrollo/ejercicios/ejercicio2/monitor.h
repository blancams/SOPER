/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado la cabecera para la implementacion
 * de la libreria del comportamiento del proceso monitor.
 *
 * @file monitor.h
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#ifndef MONITOR_H
#define MONITOR_H

#define OK 2						/*!< Valor para correcto retorno de funciones */
#define ERROR -1					/*!< Valor para incorrecto retorno de funciones */

/**
 * @brief Funcion que modela el comportamiento principal del monitor.
 *
 * @param int *shmid_apuestas: Identificadores de las zonas de memoria compartida para las apuestas.
 * @param int shmid_posiciones: Identificador para la zona de memoria compartida para las posiciones de caballos.
 * @param int n_caballos: Numero de caballos.
 * @param int semid: Identificador del semaforo para acceso a regiones compartidas.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int monitor(int *shmid_apuestas, int shmid_posiciones, int n_caballos, int n_apostadores, int semid);

/**
 * @brief Funcion que imprime la informacion de la carrera.
 *
 * @param char *estado: Mensaje con el estado de la carrera.
 * @param int n_caballos: Numero de caballos.
 * @param int *posiciones: Posiciones de los caballos.
 * @param double *cotizaciones: Cotizaciones de cada caballo.
 */
void imprimir_carrera(char *estado, int n_caballos, int *posiciones, double *cotizaciones);

/**
 * @brief Funcion que imprime la informacion de la carrera una vez finalizada.
 *
 * @param int n_caballos: Numero de caballos.
 * @param int n_apostadores: Numero de apostadores.
 * @param int *posiciones: Posiciones de los caballos.
 * @param double **ganancia: Ganancia de cada uno de los apostadores.
 */
 void imprimir_finalizada(int n_caballos, int n_apostadores, int *posiciones, double **ganancia);

/**
 * @brief Libera los recursos reservados por el proceso monitor.
 *
 * @param apuestas_total *apuestas: Argumento para la desvinculacion de la memoria compartida de las apuestas.
 * @param int *posiciones: Argumento para la desvinculacion de la memoria compartida de las posiciones de los caballos.
 * @param int n_apostadores: Numero de apostadores.
 */
 void libera_recursos_monitor(apuestas_total *apuestas, int *posiciones, int n_apostadores);

#endif
