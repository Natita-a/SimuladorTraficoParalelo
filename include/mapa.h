#ifndef MAPA_H
#define MAPA_H

#include "coordenada.h"
#include "semaforo.h"

#define FILAS 12
#define COLUMNAS 12

#define NUM_SEMAFOROS 20

#define MAX_RUTA 100
#define MAX_NOMBRE 32

typedef struct {
    int id;
    Coordenada posicion;
    int ocupada;
    int tiene_semaforo;
    int id_semaforo;
} Interseccion;

typedef struct {
    Interseccion grid[FILAS][COLUMNAS];
    char calles_h[FILAS][MAX_NOMBRE];
    char calles_v[COLUMNAS][MAX_NOMBRE];
    Semaforo semaforos[NUM_SEMAFOROS];
    int num_semaforos;
} Ciudad;

void ciudad_init(Ciudad *c);
void ciudad_imprimir(const Ciudad *c);
void actualizar_semaforos(Ciudad *c);

#endif // !MAPA_H
