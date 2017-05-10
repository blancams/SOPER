#ifndef MEMCOMP_H
#define MEMCOMP_H

#define OK 2
#define ERR -1

int crear_shm(int size, int *shmid, int key);

int acceder_shm(int shmid, char* addr);

int salir_shm(char* addr);

int eliminar_shm(int shmid);

#endif
