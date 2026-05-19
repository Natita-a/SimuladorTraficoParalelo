// src/main.c
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <omp.h>

#include "../include/auto.h"
#include "../include/config.h"
#include "../include/mapa.h"
#include "../include/sleep.h"

#define MAX_BORDES 20

int main(int argc, char **argv) {
    srand((unsigned)time(NULL));

    const char *config_path = (argc > 1) ? argv[1] : DEFAULT_CONFIG_PATH;

    Config cfg = config_defaults();
    cfg = config_from_file(config_path, cfg);
    config_print(&cfg, config_path);

    Ciudad ciudad = Ciudad_new(&cfg);
    Ciudad_imprimir(&ciudad);

    Coordenada bordes[MAX_BORDES] = {
        {0, 0},  {0, 3},  {0, 6},   {0, 9}, {0, 11}, {11, 0}, {11, 3},
        {11, 6}, {11, 9}, {11, 11}, {3, 0}, {6, 0},  {9, 0},  {3, 11},
        {6, 11}, {9, 11}, {0, 1},   {0, 5}, {11, 2}, {11, 7}};

    size_t num_autos = cfg.num_vehicles;
    if (num_autos > MAX_VEHICLES)
        num_autos = MAX_VEHICLES;

    Auto autos[MAX_VEHICLES];

    Coordenada destinos_fijos[MAX_VEHICLES] = {
        {11, 11}, {11, 9}, {11, 6}, {11, 3}, {11, 0}, {9, 11}, {6, 11}, {3, 11},
        {0, 11},  {0, 9},  {0, 6},  {0, 3},  {0, 0},  {6, 0},  {9, 0}};

    // Parallel route calculation: each Auto_new call is independent.
    // calcular_ruta uses only thread-local stack memory and reads immutable
    // grid fields, so no synchronization is needed for the Dijkstra itself.
    // The only shared write is ciudad.grid[origen].ocupada, protected below.
#pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < num_autos; i++) {
        Coordenada origen = bordes[i % MAX_BORDES];
        Coordenada destino = destinos_fijos[i % MAX_VEHICLES];
        autos[i] = Auto_new(&ciudad, (int)i, origen, destino);

        // Mark the starting cell as occupied; multiple vehicles may share an
        // origin (e.g., bordes[0] repeats every 20 cars), so use the lock.
        omp_set_lock(&ciudad.grid_locks[origen.x][origen.y]);
        ciudad.grid[origen.x][origen.y].ocupada = true;
        omp_unset_lock(&ciudad.grid_locks[origen.x][origen.y]);

#pragma omp critical(stdout)
        printf("Auto %zu: (%d,%d) -> (%d,%d)\n", i, origen.x, origen.y,
               destino.x, destino.y);
    }

    printf("\n--- SIMULACION ---\n");

    int paso;
    int todos_llegaron = 0;

    for (paso = 0; !todos_llegaron; paso++) {
        printf("\n== Paso %d ==\n", paso);

        // Phase 1 — semaphore threads (internally parallel, implicit barrier on
        // exit)
        Ciudad_actualizar_semaforos(&ciudad);

        // Phase 2 — vehicle threads (destination cell locked inside
        // Auto_update)
#pragma omp parallel for schedule(dynamic)
        for (size_t i = 0; i < num_autos; i++)
            Auto_update(&autos[i], &ciudad);

        // Phase 3 — reset occupancy (safe: no vehicles running concurrently)
#pragma omp parallel for collapse(2) schedule(static)
        for (int i = 0; i < FILAS; i++)
            for (int j = 0; j < COLUMNAS; j++)
                ciudad.grid[i][j].ocupada = false;

        sleep_ms(1000);

        // Check termination with a min-reduction (0 = some vehicle still
        // active)
        todos_llegaron = 1;
#pragma omp parallel for reduction(min : todos_llegaron)
        for (size_t i = 0; i < num_autos; i++) {
            if (autos[i].activo)
                todos_llegaron = 0;
        }
    }

    printf("\n--- FIN: todos llegaron en %d pasos ---\n", paso);

    Ciudad_destroy(&ciudad);
    return EXIT_SUCCESS;
}
