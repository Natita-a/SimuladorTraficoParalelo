#ifndef INTERSECCION_H
#define INTERSECCION_H

#include <stdbool.h>
#include <stddef.h>

#include "coordenada.h"
#include "semaforo.h"

typedef struct {
    int id;
    Coordenada posicion;
    Semaforo semaforos[4];
    int ocupada;
    bool tiene_semaforo;
    int id_semaforo;
} Interseccion;

Interseccion *Interseccion_new(int id, Coordenada posicion,
                               Semaforo semaforos[], size_t n);

#endif // !INTERSECCION_H
