
#include "interseccion.h"
#include <stdlib.h>
#include <string.h>

Interseccion *Interseccion_new(int id, Coordenada posicion,
                               Semaforo semaforos[], size_t n) {
    Interseccion *interseccion =
        malloc(sizeof(Interseccion) + n * sizeof(Semaforo));
    if (!interseccion)
        return NULL;

    interseccion->id = id;
    interseccion->posicion = posicion;
    memcpy(interseccion->semaforos, semaforos, n * sizeof(Semaforo));

    return interseccion;
}
