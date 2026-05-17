#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <stdint.h>

#include "coordenada.h"

typedef enum { ROJO, AMARILLO, VERDE } EstadoSemaforo;
typedef enum { NORTE, SUR, ESTE, OESTE } Direccion;

typedef struct {
    uint8_t id;
    EstadoSemaforo estado;
    Direccion direccion;
    uint8_t duracion_verde;
    uint8_t duracion_amarillo;
    uint8_t duracion_rojo;
    uint8_t timer;
    Coordenada posicion;
} Semaforo;

// Contructor
Semaforo Semaforo_new(uint8_t id, EstadoSemaforo estado, Direccion direccion,
                      uint8_t duracion_verde, uint8_t duracion_amarillo,
                      uint8_t duracion_rojo, uint8_t timer,
                      Coordenada posicion);

#endif // !COORD_H
