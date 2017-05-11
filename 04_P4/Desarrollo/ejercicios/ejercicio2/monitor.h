#ifndef MONITOR_H
#define MONITOR_H

#define OK 2
#define ERROR -1

int monitor(int shmid_apuestas, int shmid_posiciones, int *posiciones, int n_caballos);

/**
 * @brief Funcion que imprime la informacion de la carrera.
 *
 * @param char *estado: Mensaje con el estado de la carrera.
 * @param int n_caballos: Numero de caballos.
 * @param int *posiciones: Posiciones de los caballos.
 * @param double *cotizaciones: Cotizaciones de cada caballo.
 */
void imprimir_carrera(char *estado, int n_caballos, int *posiciones, double *cotizaciones);

/**
 * @brief Funcion que imprime la informacion de la carrera una vez finalizada.
 *
 * @param int n_caballos: Numero de caballos.
 * @param int *posiciones: Posiciones de los caballos.
 * @param double *ganancia: Ganancia de cada uno de los apostadores.
 */
 void imprimir_finalizada(int n_caballos, int *posiciones, double *ganancia);

 void libera_recursos_monitor(apuestas_total *apuestas, int *posiciones);

#endif
