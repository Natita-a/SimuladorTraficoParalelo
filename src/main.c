#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "auto.h"
#include "config.h"
#include "mapa.h"
#include "sleep.h"

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
    {11,11},
    {11,9},
    {11,6},
    {11,3},
    {11,0},

    {9,11},
    {6,11},
    {3,11},
    {0,11},

    {0,9},
    {0,6},
    {0,3},
    {0,0},

    {6,0},
    {9,0}
    };

    for (size_t i = 0; i < num_autos; i++) {
        Coordenada origen = bordes[i % MAX_BORDES];

       /* Coordenada destino;
        do {
            destino = bordes[rand() % MAX_BORDES];
        } while (destino.x == origen.x && destino.y == origen.y);
*/   

       Coordenada destino = destinos_fijos[i];

        autos[i] = Auto_new(&ciudad, (int)i, origen, destino);
        ciudad.grid[origen.x][origen.y].ocupada = true;

        printf("Auto %zu: (%d,%d) -> (%d,%d)\n", i, origen.x, origen.y,
               destino.x, destino.y);
    }

    printf("\n--- SIMULACION ---\n");

    int paso = 0;
    int todos_llegaron = 0;
    //Calcular congestion
    int congestion[FILAS][COLUMNAS] = {0};

    while (!todos_llegaron) {
        paso++;
        printf("\n== Paso %d ==\n", paso);

        Ciudad_actualizar_semaforos(&ciudad);

       /* for (size_t i = 0; i < num_autos; i++)
            Auto_update(&autos[i], &ciudad);*/

            //Congestion
        for (size_t i = 0; i < num_autos; i++) {

          int idx_antes = autos[i].idx;

          Coordenada actual =
          (autos[i].idx > 0)
          ? autos[i].ruta[autos[i].idx - 1]
         : autos[i].origen;

         Auto_update(&autos[i], &ciudad);

        if (autos[i].activo && autos[i].idx == idx_antes) {
        congestion[actual.x][actual.y]++;
         }
        }

        for (size_t i = 0; i < FILAS; i++) {
            for (size_t j = 0; j < COLUMNAS; j++)
                ciudad.grid[i][j].ocupada = false;
        }

        sleep_ms(1000);

        todos_llegaron = 1;
        for (size_t i = 0; i < num_autos; i++) {
            if (autos[i].activo) {
                todos_llegaron = 0;
                break;
            }
        }
    }
    
    printf("\n=== CONGESTION POR INTERSECCION ===\n");

    for (int i = 0; i < FILAS; i++) {
       for (int j = 0; j < COLUMNAS; j++) {

        if (congestion[i][j] > 0) {

            printf("Interseccion (%d,%d): %d vehiculos en espera\n",
                   i, j, congestion[i][j]);
        }
      }
   }
    printf("\n--- FIN: todos llegaron en %d pasos ---\n", paso);
    return EXIT_SUCCESS;
}
