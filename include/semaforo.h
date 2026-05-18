#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <stdint.h>

#include "coordenada.h"

typedef enum { VERDE = 0, AMARILLO, ROJO } EstadoSemaforo;

typedef enum { NORTE = 0, SUR, ESTE, OESTE } Direccion;

typedef struct {
    uint8_t id;
    EstadoSemaforo estado;
    Direccion direccion;
    uint16_t duracion_verde;
    uint16_t duracion_amarillo;
    uint16_t duracion_rojo;
    uint16_t timer;
    Coordenada posicion;
} Semaforo;

Semaforo Semaforo_new(uint8_t id, EstadoSemaforo estado, Direccion direccion,
                      uint16_t duracion_verde, uint16_t duracion_amarillo,
                      uint16_t duracion_rojo, uint16_t timer,
                      Coordenada posicion);

#endif // SEMAFORO_H
