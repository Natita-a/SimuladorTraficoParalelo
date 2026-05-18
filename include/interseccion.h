#ifndef INTERSECCION_H
#define INTERSECCION_H

#include <stdbool.h>

#include "coordenada.h"

typedef struct {
    int id;
    Coordenada posicion;
    bool ocupada;
    bool tiene_semaforo;
    int id_semaforo;
} Interseccion;

Interseccion Interseccion_new(int id, Coordenada posicion);

#endif // INTERSECCION_H
