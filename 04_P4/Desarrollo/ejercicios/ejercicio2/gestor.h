#ifndef GESTOR_H
#define GESTOR_H

#define OK 2
#define ERROR -1

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
   double *ganancia;
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

int gestor(int shmid_apuestas, int semid, int n_apostadores, int n_caballos,
   int n_ventanillas, int key, pid_t monitor, pid_t apostador);

/**
 * @brief Funcion que recoge el comportamiento de cada uno de los hilos ventanilla.
 *
 * @param void *args: Argumentos que contienen una estructura de tipo str_ventanilla.
 */
void *ventanilla(void *arg);

void libera_recursos_gestor(apuestas_total *apuestas, pthread_t *hilos);

#endif
