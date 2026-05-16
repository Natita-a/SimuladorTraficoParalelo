#include <stdio.h>
#include "auto.h"

#define INFINITO 99999


static int calcular_ruta(Ciudad *c, Auto *a) {

    int dist[FILAS][COLUMNAS];
    int procesado[FILAS][COLUMNAS];
    Coord padre[FILAS][COLUMNAS];

    int dx[] = {-1, 1,  0, 0};
    int dy[] = { 0, 0, -1, 1};


    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            dist[i][j]      = INFINITO;
            procesado[i][j] = 0;
            padre[i][j].x   = -1;
            padre[i][j].y   = -1;
        }
    }


    dist[a->origen.x][a->origen.y] = 0;

    /* Dijkstra */
    for (int iter = 0; iter < FILAS * COLUMNAS; iter++) {

        /* Buscar el nodo no procesado con menor distancia */
        Coord u;
        u.x = -1;
        u.y = -1;
        int menor = INFINITO;

        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                if (!procesado[i][j] && dist[i][j] < menor) {
                    menor = dist[i][j];
                    u.x = i;
                    u.y = j;
                }
            }
        }

        /* Si no encontro nodo valido, terminar */
        if (u.x == -1) break;

        /* Si llegamos al destino, terminar */
        if (u.x == a->destino.x && u.y == a->destino.y)
            break;

        procesado[u.x][u.y] = 1;

        /* Revisar los 4 vecinos */
        for (int d = 0; d < 4; d++) {
            int nx = u.x + dx[d];
            int ny = u.y + dy[d];

            if (nx < 0 || nx >= FILAS ||
                ny < 0 || ny >= COLUMNAS)
                continue;

            if (procesado[nx][ny]) continue;

            /* Peso: 2 si tiene semaforo, 1 si no */
            int peso = 1;
            if (c->grid[nx][ny].tiene_semaforo)
                peso = 2;

            int nueva_dist = dist[u.x][u.y] + peso;

            if (nueva_dist < dist[nx][ny]) {
                dist[nx][ny]    = nueva_dist;
                padre[nx][ny].x = u.x;
                padre[nx][ny].y = u.y;
            }
        }
    }

    /* Verificar si llego al destino */
    if (dist[a->destino.x][a->destino.y] == INFINITO) {
        printf("Auto %d: no se encontro ruta\n", a->id);
        return 0;
    }

    /* Reconstruir ruta de atras hacia adelante */
    Coord temp[MAX_RUTA];
    int n = 0;
    Coord cur = a->destino;

    while (!(cur.x == a->origen.x &&
             cur.y == a->origen.y)) {
        temp[n++] = cur;
        Coord p   = padre[cur.x][cur.y];
        cur        = p;
        if (n >= MAX_RUTA) break;
    }
    temp[n++] = a->origen;

    /* Invertir para que quede origen -> destino */
    a->tam_ruta = n;
    for (int i = 0; i < n; i++)
        a->ruta[i] = temp[n - 1 - i];

    return 1;
}

/* CREAR AUTO*/
void crear_auto(Ciudad *c, Auto *a, int id, Coord origen, Coord destino) {
    a->id       = id;
    a->origen   = origen;
    a->destino  = destino;
    a->tam_ruta = 0;
    a->idx      = 0;
    a->activo   = 1;

    calcular_ruta(c, a);
}

/*ACTUALIZAR AUTO*/
void actualizar_auto(Ciudad *c, Auto *a) {


    if (!a->activo) return;


    if (a->idx >= a->tam_ruta) {
        a->activo = 0;
        printf("Auto %d llego a su destino (%d,%d)\n",
               a->id, a->destino.x, a->destino.y);
        return;
    }

    Coord sig = a->ruta[a->idx];
    Interseccion *inter = &c->grid[sig.x][sig.y];

    /* Verificar semaforo */
    if (inter->tiene_semaforo) {
        Semaforo *s = &c->semaforos[inter->id_semaforo];
        if (s->estado == ROJO || s->estado == AMARILLO) {
            printf("Auto %d esperando semaforo en (%d,%d)\n",
                   a->id, sig.x, sig.y);
            return;
        }
    }

    /* Verificar si la interseccion esta ocupada */
    if (inter->ocupada) {
        printf("Auto %d esperando interseccion (%d,%d)\n",
               a->id, sig.x, sig.y);
        return;
    }

    /* Liberar interseccion anterior */
    if (a->idx > 0) {
        Coord ant = a->ruta[a->idx - 1];
        c->grid[ant.x][ant.y].ocupada = 0;
    }

    /* Mover el auto */
    inter->ocupada = 1;
    printf("Auto %d se movio a (%d,%d)\n",
           a->id, sig.x, sig.y);

    a->idx++;
}
