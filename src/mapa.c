#include <stdint.h>
#include <stdio.h>

#include "mapa.h"

/* =========================
   POSICIONES DE SEMAFOROS
   ========================= */
static const Coordenada POS_SEMAFOROS[NUM_SEMAFOROS] = {
    {0, 5}, {0, 10}, {1, 2}, {1, 7}, {2, 4},  {2, 10}, {3, 1},
    {3, 6}, {4, 3},  {4, 9}, {5, 5}, {5, 10}, {6, 2},  {6, 8},
    {7, 4}, {7, 10}, {8, 6}, {9, 1}, {9, 9},  {11, 5}};

/* =========================
   CREAR CIUDAD
   ========================= */

Ciudad Ciudad_new(void) {
    Ciudad ciudad = {
        .calles_h = {"Av. Juarez", "Av. Cardenas", "Av. Porfirio Diaz",
                     "Av. Madero", "Av. Carranza", "Av. Obregon", "Av. Calles",
                     "Av. Avila Camacho", "Av. Aleman", "Av. Ruiz Cortines",
                     "Av. Lopez Mateos", "Av. Echeverria"},
        .calles_v = {"C. Gpe. Victoria", "C. Bustamante", "C. Pedraza",
                     "C. Santa Anna", "C. Herrera", "C. Paredes", "C. Arista",
                     "C. De la Fuente", "C. Comonfort", "C. Lerdo",
                     "Av. Gonzalez", "C. Manuel Gonzalez"},
        .num_semaforos = NUM_SEMAFOROS,
    };

    /* Inicializar intersecciones */
    int id = 0;
    for (int fila = 0; fila < FILAS; fila++) {
        for (int columna = 0; columna < COLUMNAS; columna++) {
            ciudad.grid[fila][columna] = (Interseccion){
                .id = id++,
                .posicion = {.x = fila, .y = columna},
                .ocupada = 0,
                .tiene_semaforo = false,
                .id_semaforo = -1,
            };
        }
    }

    /* Crear semaforos */
    for (int i = 0; i < NUM_SEMAFOROS; i++) {
        ciudad.semaforos[i] = (Semaforo){
            .id = i,
            .posicion = POS_SEMAFOROS[i],
            .estado = VERDE,
            .direccion = NORTE,
            .duracion_verde = 5,
            .duracion_amarillo = 2,
            .duracion_rojo = 6,
            .timer = 0,
        };

        Interseccion *inter = &ciudad.grid[ciudad.semaforos[i].posicion.x]
                                          [ciudad.semaforos[i].posicion.y];
        inter->tiene_semaforo = 1;
        inter->id_semaforo = i;
    }

    return ciudad;
}
/* =========================
   IMPRIMIR MAPA
   ========================= */
void Ciudad_imprimir(const Ciudad *c) {
    printf("\n=== MAPA DE LA CIUDAD ===\n\n");

    for (int fila = 0; fila < FILAS; fila++) {
        for (int columna = 0; columna < COLUMNAS; columna++) {
            const Interseccion *inter = &c->grid[fila][columna];

            if (inter->tiene_semaforo)
                printf(" [S]");
            else
                printf(" [+]");
        }
        printf("\n");
    }
    printf("\n");
}

/* =========================
   ACTUALIZAR SEMAFOROS
   ========================= */
void Ciudad_actualizar_semaforos(Ciudad *c) {
    for (int i = 0; i < c->num_semaforos; i++) {
        Semaforo *s = &c->semaforos[i];

        s->timer++;

        switch (s->estado) {
            case VERDE:
                if (s->timer >= s->duracion_verde) {
                    s->estado = AMARILLO;
                    s->timer = 0;
                }
                break;
            case AMARILLO:
                if (s->timer >= s->duracion_amarillo) {
                    s->estado = ROJO;
                    s->timer = 0;
                }
                break;
            case ROJO:
                if (s->timer >= s->duracion_rojo) {
                    s->estado = VERDE;
                    s->timer = 0;
                }
                break;
        }
    }
}
