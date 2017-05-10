#ifndef HILOS_H
#define HILOS_H

#define OK 2
#define ERROR -1

int crear_hilo(pthread_t *hilo, void*(*funcion)(void*), void *args);

int unir_hilo(pthread_t hilo);

int salir_hilo(pthread_t hilo);

#endif
