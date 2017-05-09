#ifndef MENSAJES_H
#define MENSAJES_H

#define OK 2
#define ERROR -1
#define MAX_CHAR 512

/*
typedef struct _Mensaje {
   long tipo;
   char mensaje[MAX_CHAR];
} mensaje;
*/ 

int crear_cm(int *cmid, int key);

//int enviar_m(int msid, char *mensaje, long tipo);
int enviar_m(int msid, const void *mensaje);

//int recibir_m(int msid, long tipo);
int recibir_m(int msid, const void *mensaje, long tipo);

int eliminar_cm(int msid);

#endif
