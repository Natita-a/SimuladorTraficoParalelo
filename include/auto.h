#ifndef AUTO_H
#define AUTO_H

#include "coordenada.h"
#include "mapa.h"

#define MAX_RUTA 100

typedef struct {
    int id;
    Coordenada origen;
    Coordenada destino;
    Coordenada ruta[MAX_RUTA];
    int tam_ruta;
    int idx;
    int activo;
} Auto;

Auto Auto_new(Ciudad *ciudad, int id, Coordenada origen, Coordenada destino);
void Auto_update(Auto *a);

#endif // !AUTO_H
