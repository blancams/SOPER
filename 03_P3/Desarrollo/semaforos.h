#ifndef SEMAFOROS_H
#define SEMAFOROS_H

#define OK 2        
#define ERROR -1

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
 * @param key_t key: Clave precompartida del semaforo.
 * @param int size: Tamaño del semaforo.
 * @param int *semid: Nuevo identificador del semaforo.
 * @return int: ERROR en caso de error,
              0 si ha creado el semaforo,
              1 si ya estaba creado.
 */
int Crear_Semaforo(key_t key, int size, int *semid);

<<<<<<< HEAD
/**
 * @brief Baja el semaforo indicado.
 *
 * @param int semid: Identificador del semaforo.
 * @param int num_sem: Semaforo dentro del array.
 * @param int undo: Flag de modo persistente pese a finalización abrupta.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int Down_Semaforo(int id, int num_sem, int undo);

/**
 * @brief Baja todos los semaforos del array indicado por active.
 *
 * @param int semid: Identificador del semaforo.
 * @param int size: Numero de semaforos del array.
 * @param int undo: Flag de modo persistente pese a finalización abrupta.
 * @param int *active: Semaforos involucrados.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int DownMultiple_Semaforo(int id,int size,int undo,int *active);

/**
 * @brief Sube el semaforo indicado.
 *
 * @param int semid: Identificador del semaforo.
 * @param int num_sem: Semaforo dentro del array.
 * @param int undo: Flag de modo persistente pese a finalización abrupta.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
 int Up_Semaforo(int id, int num_sem, int undo);

/**
 * @brief Sube todos los semaforos del array indicado por active.
 *
 * @param int semid: Identificador del semaforo.
 * @param int size: Numero de semaforos del array.
 * @param int undo: Flag de modo persistente pese a finalización abrupta.
 * @param int *active: Semaforos involucrados.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */
int UpMultiple_Semaforo(int id,int size, int undo, int *active);
=======
/**************************************************************
     Nombre:Down_Semaforo
     Descripcion: Baja el semaforo indicado
     Entrada:
        int semid: Identificador del semaforo.
        int num_sem: Semaforo dentro del array.
        int undo: Flag de modo persistente pese a finalización abrupta.
     Salida:
       int: OK si todo fue correcto, ERROR en caso de error.
***************************************************************/
int Down_Semaforo(int id, int num_sem, short undo);

/***************************************************************
     Nombre: DownMultiple_Semaforo
     Descripcion: Baja todos los semaforos del array indicado
                 por active.
     Entrada:
        int semid: Identificador del semaforo.
        int size: Numero de semaforos del array.
        int undo: Flag de modo persistente pese a finalización abrupta.
        int *active: Semaforos involucrados.
     Salida:
          int: OK si todo fue correcto, ERROR en caso de error.
***************************************************************/
int DownMultiple_Semaforo(int id,int size,short undo,int *active);

/**************************************************************
     Nombre:Up_Semaforo
     Descripcion: Sube el semaforo indicado
     Entrada:
        int semid: Identificador del semaforo.
        int num_sem: Semaforo dentro del array.
        int undo: Flag de modo persistente pese a finalizacion abupta.
     Salida:
       int: OK si todo fue correcto, ERROR en caso de error.
***************************************************************/
int Up_Semaforo(int id, int num_sem, short undo);

/***************************************************************
     Nombre: UpMultiple_Semaforo
     Descripcion: Sube todos los semaforos del array indicado por active.
     Entrada:
        int semid: Identificador del semaforo.
        int size: Numero de semaforos del array.
        int undo: Flag de modo persistente pese a finalización abrupta.
        int *active: Semaforos involucrados.
     Salida:
          int: OK si todo fue correcto, ERROR en caso de error.
***************************************************************/
int UpMultiple_Semaforo(int id,int size, short undo, int *active);
>>>>>>> ebb472873fef906ba3a9ee9753e121c81ca779ba

#endif
