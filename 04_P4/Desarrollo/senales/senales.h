#ifndef SENALES_H
#define SENALES_H

#define OK 2
#define ERROR -1

int enviar_senal(pid_t proceso, int senal);

int crear_manej(int senal, void *funcion);

#endif
