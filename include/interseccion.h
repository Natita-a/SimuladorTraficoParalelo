#ifndef INTERSECCION_H
#define INTERSECCION_H

#include <stddef.h>

#include "coordenada.h"
#include "semaforo.h"

typedef struct {
    int id;
    Coordenada posicion;
    Semaforo semaforos[];
} Interseccion;

Interseccion *Interseccion_new(int id, Coordenada posicion,
                               Semaforo semaforos[], size_t n);

#endif // !INTERSECCION_H
