#include <stdio.h>
#include <string.h>

#include "mapa.h"

/* =========================
   POSICIONES DE SEMAFOROS
   ========================= */
static const Coord POS_SEMAFOROS[NUM_SEMAFOROS] = {

    {0, 5}, {0, 10}, {1, 2}, {1, 7}, {2, 4},  {2, 10}, {3, 1},
    {3, 6}, {4, 3},  {4, 9}, {5, 5}, {5, 10}, {6, 2},  {6, 8},
    {7, 4}, {7, 10}, {8, 6}, {9, 1}, {9, 9},  {11, 5}};

/* =========================
   INICIALIZAR CIUDAD
   ========================= */
void ciudad_init(Ciudad *c) {

    /* Calles horizontales */
    strcpy(c->calles_h[0], "Av. Juarez");
    strcpy(c->calles_h[1], "Av. Cardenas");
    strcpy(c->calles_h[2], "Av. Porfirio Diaz");
    strcpy(c->calles_h[3], "Av. Madero");
    strcpy(c->calles_h[4], "Av. Carranza");
    strcpy(c->calles_h[5], "Av. Obregon");
    strcpy(c->calles_h[6], "Av. Calles");
    strcpy(c->calles_h[7], "Av. Avila Camacho");
    strcpy(c->calles_h[8], "Av. Aleman");
    strcpy(c->calles_h[9], "Av. Ruiz Cortines");
    strcpy(c->calles_h[10], "Av. Lopez Mateos");
    strcpy(c->calles_h[11], "Av. Echeverria");

    /* Calles verticales */
    strcpy(c->calles_v[0], "C. Gpe. Victoria");
    strcpy(c->calles_v[1], "C. Bustamante");
    strcpy(c->calles_v[2], "C. Pedraza");
    strcpy(c->calles_v[3], "C. Santa Anna");
    strcpy(c->calles_v[4], "C. Herrera");
    strcpy(c->calles_v[5], "C. Paredes");
    strcpy(c->calles_v[6], "C. Arista");
    strcpy(c->calles_v[7], "C. De la Fuente");
    strcpy(c->calles_v[8], "C. Comonfort");
    strcpy(c->calles_v[9], "C. Lerdo");
    strcpy(c->calles_v[10], "Av. Gonzalez");
    strcpy(c->calles_v[11], "C. Manuel Gonzalez");

    /* Inicializar intersecciones */
    int id = 0;

    for (int fila = 0; fila < FILAS; fila++) {

        for (int columna = 0; columna < COLUMNAS; columna++) {

            Interseccion *inter = &c->grid[fila][columna];

            inter->id = id++;

            inter->posicion.x = fila;
            inter->posicion.y = columna;

            inter->ocupada = 0;

            inter->tiene_semaforo = 0;

            inter->id_semaforo = -1;
        }
    }

    /* Crear semaforos */
    c->num_semaforos = NUM_SEMAFOROS;

    for (int i = 0; i < NUM_SEMAFOROS; i++) {

        Semaforo *s = &c->semaforos[i];

        s->id = i;

        s->posicion = POS_SEMAFOROS[i];

        s->estado = VERDE;

        s->direccion = NORTE;

        s->t_verde = 5;
        s->t_amarillo = 2;
        s->t_rojo = 6;

        s->timer = 0;

        Interseccion *inter = &c->grid[s->posicion.x][s->posicion.y];

        inter->tiene_semaforo = 1;

        inter->id_semaforo = i;
    }
}

/* =========================
   IMPRIMIR MAPA
   ========================= */
void ciudad_imprimir(const Ciudad *c) {

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
void actualizar_semaforos(Ciudad *c) {

    for (int i = 0; i < c->num_semaforos; i++) {

        Semaforo *s = &c->semaforos[i];

        s->timer++;

        if (s->estado == VERDE && s->timer >= s->t_verde) {

            s->estado = AMARILLO;

            s->timer = 0;
        }

        else if (s->estado == AMARILLO && s->timer >= s->t_amarillo) {

            s->estado = ROJO;

            s->timer = 0;
        }

        else if (s->estado == ROJO && s->timer >= s->t_rojo) {

            s->estado = VERDE;

            s->timer = 0;
        }
    }
}
