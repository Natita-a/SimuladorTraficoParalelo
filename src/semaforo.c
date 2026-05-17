#include "semaforo.h"
#include <stdint.h>

Semaforo Semaforo_new(uint8_t id, EstadoSemaforo estado, Direccion direccion,
                      uint8_t duracion_verde, uint8_t duracion_amarillo,
                      uint8_t duracion_rojo, uint8_t timer,
                      Coordenada posicion) {
    return (Semaforo){
        .id = id,
        .estado = estado,
        .direccion = direccion,
        .duracion_verde = duracion_verde,
        .duracion_amarillo = duracion_amarillo,
        .duracion_rojo = duracion_rojo,
        .timer = timer,
        .posicion = posicion,
    };
}
