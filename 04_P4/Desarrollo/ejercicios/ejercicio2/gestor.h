/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado la cabecera para la implementacion
 * de la libreria del comportamiento del proceso gestor.
 *
 * @file gestor.h
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#ifndef GESTOR_H
#define GESTOR_H

#define OK 2						/*!< Valor para correcto retorno de funciones */
#define ERROR -1					/*!< Valor para incorrecto retorno de funciones */

/**
 * @brief Estructura para saber en todo momento los datos de las apuestas.
 *
 * Estructura que contiene lo apostado por cada caballo, su cotizacion, el total y la ganancia
 * de cada apostador.
 */
typedef struct _Apuestas_Total{
   double *apostado;
   double *cotizacion;
   double total;
   double **ganancia;
} apuestas_total;

/**
 * @brief Estructura para mandar como argumento del gestor a los hilos ventanilla.
 *
 * Estructura que contiene el identificador de la cola de mensajes, el identificador
 * del semaforo y todas las apuestas.
 */
typedef struct _Gestor_Ventanilla{
   int msqid;
   int semid;
   apuestas_total *apuestas;
} str_ventanilla;

/**
 * @brief Funcion que modela el comportamiento principal del gestor de apuestas.
 *
 * @param int *shmid_apuestas: Identificadores de las regiones de memoria compartida sobre las apuestas.
 * @param int semid: Identificador del semaforo mutex para acceso a los datos compartidos.
 * @param int n_apostadores: Numero de apostadores.
 * @param int n_caballos: Numero de caballos.
 * @param int n_ventanillas: Numero de ventanillas.
 * @param int key: Clave para el acceso a la cola de mensajes.
 * @param pid_t monitor: Identificador del proceso monitor.
 * @param pid_t apostador: Identificador del proceso apostador.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int gestor(int *shmid_apuestas, int semid, int n_apostadores, int n_caballos,
   int n_ventanillas, int key, pid_t monitor, pid_t apostador);

/**
 * @brief Funcion que recoge el comportamiento de cada uno de los hilos ventanilla.
 *
 * @param void *args: Argumentos que contienen una estructura de tipo str_ventanilla.
 */
void *ventanilla(void *arg);

/**
 * @brief Funcion que libera los recursos del proceso gestor.
 *
 * @param pthread_t *hilos: Array de hilos a liberar.
 */
void libera_recursos_gestor(pthread_t *hilos);

#endif
