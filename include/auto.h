#ifndef AUTO_H_INCLUDED
#define AUTO_H_INCLUDED

#include "mapa.h"

#define MAX_RUTA 100

typedef struct {
    int id;

    Coord origen;
    Coord destino;

    Coord ruta[MAX_RUTA];
    int tam_ruta;
    int idx;

    int activo;
} Auto;

void crear_auto(Ciudad *c, Auto *a, int id, Coord origen, Coord destino);
void actualizar_auto(Ciudad *c, Auto *a);

#endif
