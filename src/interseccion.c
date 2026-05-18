#include "interseccion.h"

Interseccion Interseccion_new(int id, Coordenada posicion) {
    return (Interseccion){
        .id = id,
        .posicion = posicion,
        .ocupada = false,
        .tiene_semaforo = false,
        .id_semaforo = -1,
    };
}
