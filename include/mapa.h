#ifndef MAPA_H
#define MAPA_H

#include "coordenada.h"
#include "interseccion.h"
#include "semaforo.h"

#define FILAS 12
#define COLUMNAS 12

#define NUM_SEMAFOROS 20

#define MAX_RUTA 100
#define MAX_NOMBRE 32

typedef struct {
    Interseccion grid[FILAS][COLUMNAS];
    const char *calles_h[FILAS];
    const char *calles_v[COLUMNAS];
    Semaforo semaforos[NUM_SEMAFOROS];
    int num_semaforos;
} Ciudad;

Ciudad Ciudad_new(void);
void Ciudad_imprimir(const Ciudad *ciudad);
void Ciudad_actualizar_semaforos(Ciudad *ciudad);

#endif // !MAPA_H
