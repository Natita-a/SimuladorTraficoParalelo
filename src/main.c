#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "auto.h"
#include "mapa.h"

#define NUM_AUTOS 20

int main() {
    srand(time(NULL));

    /* Inicializar ciudad */
    Ciudad ciudad;
    ciudad_init(&ciudad);
    ciudad_imprimir(&ciudad);

    /* Puntos de entrada en los bordes del mapa */
    Coordenada bordes[] = {{0, 0},  {0, 3},  {0, 6},  {0, 9},  {0, 11},
                           {11, 0}, {11, 3}, {11, 6}, {11, 9}, {11, 11},
                           {3, 0},  {6, 0},  {9, 0},  {3, 11}, {6, 11},
                           {9, 11}, {0, 1},  {0, 5},  {11, 2}, {11, 7}};
    int num_bordes = 20;

    /* Crear 20 autos */
    Auto autos[NUM_AUTOS];

    for (int i = 0; i < NUM_AUTOS; i++) {
        Coordenada origen = bordes[i % num_bordes];

        Coordenada destino;
        do {
            destino = bordes[rand() % num_bordes];
        } while (destino.x == origen.x && destino.y == origen.y);

        crear_auto(&ciudad, &autos[i], i, origen, destino);

        printf("Auto %d: (%d,%d) -> (%d,%d)\n", i, origen.x, origen.y,
               destino.x, destino.y);
    }

    /* Simular hasta que todos lleguen */
    printf("\n--- SIMULACION ---\n");

    int paso = 0;
    int todos_llegaron = 0;

    while (!todos_llegaron) {
        paso++;
        printf("\n== Paso %d ==\n", paso);

        /* Actualizar semaforos */
        actualizar_semaforos(&ciudad);

        /* Mover todos los autos */
        for (int i = 0; i < NUM_AUTOS; i++)
            actualizar_auto(&ciudad, &autos[i]);

        /* Verificar si todos llegaron */
        todos_llegaron = 1;
        for (int i = 0; i < NUM_AUTOS; i++) {
            if (autos[i].activo) {
                todos_llegaron = 0;
                break;
            }
        }
    }

    printf("\n--- FIN: todos llegaron en %d pasos ---\n", paso);

    return 0;
}
