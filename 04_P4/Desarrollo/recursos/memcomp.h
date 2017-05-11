/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado la cabecera para la implementacion
 * de la libreria de trabajo con bloques de memoria compartida.
 *
 * @file memcomp.h
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#ifndef MEMCOMP_H
#define MEMCOMP_H

#define OK 2						/*!< Valor para correcto retorno de funciones */
#define ERROR -1					/*!< Valor para incorrecto retorno de funciones */

/**
 * @brief Crea una region de memoria compartida.
 *
 * @param int size: Tamaño de la region compartida que se quiere crear.
 * @param int *shmid: Puntero donde se guardara el identificador del area de memoria compartida creada.
 * @param int key: Clave que se utilizara en la creacion de la region de la memoria compartida.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int crear_shm(int size, int *shmid, int key);

/**
 * @brief Accede a una region de memoria compartida creada.
 *
 * @param int shmid: Identificador del area de memoria compartida a la que se quiere acceder.
 * @return int: Puntero al área de memoria si todo fue correcto, (void*) -1 en caso de error.
 */
void* acceder_shm(int shmid);

/**
 * @brief Desvincula las direcciones de memoria de un proceso de la region de memoria compartida.
 *
 * @param void *addr: Direccion de acceso a la memoria compartida.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int salir_shm(void* addr);

/**
 * @brief Marca para su liberacion una region de memoria compartida.
 *
 * @param int shmid: Identificador de la region de memoria compartida.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int eliminar_shm(int shmid);

#endif
