/**
 * @brief Sistemas Operativos: Practica 4, ejercicio 2
 *
 * Grupo 2201, Pareja 10.
 * En este modulo se ha implementado la cabecera para la implementacion
 * de la libreria de trabajo con mensajes.
 *
 * @file mensajes.h
 * @author Blanca Martín (blanca.martins@estudiante.uam.es)
 * @author Fernando Villar (fernando.villarg@estudiante.uam.es)
 * @date 12-05-2017
 */
#ifndef MENSAJES_H
#define MENSAJES_H

#define OK 2						/*!< Valor para correcto retorno de funciones */
#define ERROR -1					/*!< Valor para incorrecto retorno de funciones */
#define MAX_CHAR 512				/*!< Maximo numero de caracteres */

/*
typedef struct _Mensaje {
   long tipo;
   char mensaje[MAX_CHAR];
} mensaje;
*/

/**
 * @brief Crea una cola de mensajes.
 *
 * @param int *cmid: Puntero donde se guardara el identificador de la cola de mensajes creada.
 * @param int key: Clave que se utilizara en la creacion de la cola de mensajes.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int crear_cm(int *cmid, int key);

/**
 * @brief Envia un mensaje a la cola especificada.
 *
 * @param int msid: Identificador de la cola a la que se quiere enviar el mensaje.
 * @param void *mensaje: Contenido del mensaje que se quiere enviar.
 * @param int size: Tamaño del mensaje a enviar.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int enviar_m(int msid, void *mensaje, int size);

/**
 * @brief Recibe un mensaje de la cola especificada.
 *
 * @param int msid: Identificador de la cola de la que se quiere recibir un mensaje.
 * @param void *mensaje: Puntero donde se guardara el contenido del mensaje recibido.
 * @param long tipo: Tipo de mensaje que se quiere recibir.
 * @param int size: Tamaño del mensaje a recibir.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int recibir_m(int msid, void *mensaje, long tipo, int size);

/**
 * @brief Elimina una cola de mensajes.
 *
 * @param int msid: Identificador de la cola de mensajes a eliminar.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int eliminar_cm(int msid);

#endif
