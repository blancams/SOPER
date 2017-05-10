/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado la cabecera para la implementacion
 * de la llibreria de semaforos.
 *
 * @file semaforos.h
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#ifndef SEMAFOROS_H
#define SEMAFOROS_H

#define OK 2						/*!< Valor para correcto retorno de funciones */
#define ERROR -1					/*!< Valor para incorrecto retorno de funciones */

/**
 * @brief Estructura para manejo de semaforos.
 *
 * Estructura que contiene aquellos componentes necesarios para que las funciones
 * del sistema destinadas al manejo de los semaforos funcionen correctamente.
 */
union semun {
	int val;
	struct semid_ds *semstat;
	unsigned short *array;
};

/**
 * @brief Inicializa los semaforos indicados.
 *
 * @param int semid: Identificador del semaforo.
 * @param unsigned short *array: Valores iniciales.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int Inicializar_Semaforo(int semid, unsigned short *array);

/**
 * @brief Borra un semaforo.
 *
 * @param int semid: Identificador del semaforo.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int Borrar_Semaforo(int semid);

/**
 * @brief Crea un semaforo con la clave y el tamaño.
 *
 * @param int key: Clave precompartida del semaforo.
 * @param int size: Tamaño del semaforo.
 * @param int *semid: Nuevo identificador del semaforo.
 * @return int: ERROR en caso de error,
              0 si ha creado el semaforo,
              1 si ya estaba creado.
 */
int Crear_Semaforo(int key, int size, int *semid);

/**
 * @brief Baja el semaforo indicado.
 *
 * @param int semid: Identificador del semaforo.
 * @param int num_sem: Semaforo dentro del array.
 * @param int undo: Flag de modo persistente pese a finalización abrupta.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int Down_Semaforo(int id, int num_sem, short undo);

/**
 * @brief Baja todos los semaforos del array indicado por active.
 *
 * @param int semid: Identificador del semaforo.
 * @param int size: Numero de semaforos del array.
 * @param int undo: Flag de modo persistente pese a finalización abrupta.
 * @param int *active: Semaforos involucrados.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int DownMultiple_Semaforo(int id,int size,short undo,int *active);

/**
 * @brief Sube el semaforo indicado.
 *
 * @param int semid: Identificador del semaforo.
 * @param int num_sem: Semaforo dentro del array.
 * @param int undo: Flag de modo persistente pese a finalización abrupta.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int Up_Semaforo(int id, int num_sem, short undo);

/**
 * @brief Sube todos los semaforos del array indicado por active.
 *
 * @param int semid: Identificador del semaforo.
 * @param int size: Numero de semaforos del array.
 * @param int undo: Flag de modo persistente pese a finalización abrupta.
 * @param int *active: Semaforos involucrados.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int UpMultiple_Semaforo(int id,int size, short undo, int *active);

#endif
