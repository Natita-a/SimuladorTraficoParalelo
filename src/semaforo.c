#include "semaforo.h"

Semaforo Semaforo_new(uint8_t id, EstadoSemaforo estado, Direccion direccion,
                      uint16_t duracion_verde, uint16_t duracion_amarillo,
                      uint16_t duracion_rojo, uint16_t timer,
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
