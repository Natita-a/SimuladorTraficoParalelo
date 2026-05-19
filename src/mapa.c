// src/mapa.c
#include "../include/mapa.h"

#include <omp.h>
#include <stdio.h>

static const Coordenada POS_SEMAFOROS[NUM_SEMAFOROS] = {
    {0, 5}, {0, 10}, {1, 2}, {1, 7}, {2, 4},  {2, 10}, {3, 1},
    {3, 6}, {4, 3},  {4, 9}, {5, 5}, {5, 10}, {6, 2},  {6, 8},
    {7, 4}, {7, 10}, {8, 6}, {9, 1}, {9, 9},  {11, 5}};

Ciudad Ciudad_new(const Config *cfg) {
    Config defaults = config_defaults();
    if (!cfg)
        cfg = &defaults;

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

    int id = 0;
    for (int fila = 0; fila < FILAS; fila++) {
        for (int columna = 0; columna < COLUMNAS; columna++) {
            ciudad.grid[fila][columna] =
                Interseccion_new(id++, (Coordenada){.x = fila, .y = columna});
            omp_init_lock(&ciudad.grid_locks[fila][columna]);
        }
    }

    for (int i = 0; i < NUM_SEMAFOROS; i++) {
        ciudad.semaforos[i] = Semaforo_new(
            (uint8_t)i, (i % 2 == 0) ? VERDE : ROJO, NORTE, cfg->green_duration,
            cfg->yellow_duration, cfg->red_duration, 0, POS_SEMAFOROS[i]);

        Interseccion *inter =
            &ciudad.grid[POS_SEMAFOROS[i].x][POS_SEMAFOROS[i].y];
        inter->tiene_semaforo = true;
        inter->id_semaforo = i;
    }

    return ciudad;
}

void Ciudad_destroy(Ciudad *ciudad) {
    for (int i = 0; i < FILAS; i++)
        for (int j = 0; j < COLUMNAS; j++)
            omp_destroy_lock(&ciudad->grid_locks[i][j]);
}

void Ciudad_imprimir(const Ciudad *c) {
    printf("\n=== MAPA DE LA CIUDAD ===\n\n");
    for (int fila = 0; fila < FILAS; fila++) {
        for (int columna = 0; columna < COLUMNAS; columna++) {
            const Interseccion *inter = &c->grid[fila][columna];
            if (inter->tiene_semaforo) {
                const Semaforo *s = &c->semaforos[inter->id_semaforo];
                char letra = (s->estado == VERDE)      ? 'V'
                             : (s->estado == AMARILLO) ? 'A'
                                                       : 'R';
                printf(" [%c]", letra);
            } else {
                printf(" [+]");
            }
        }
        printf("\n");
    }
    printf("\n");
}

// Each semaphore runs as an independent thread — no shared writes between them
void Ciudad_actualizar_semaforos(Ciudad *c) {
#pragma omp parallel for schedule(static)
    for (int i = 0; i < c->num_semaforos; i++) {
        Semaforo *s = &c->semaforos[i];
#pragma omp atomic update
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
    // Implicit barrier: all semaphore threads finish before vehicles read
    // estado
}
