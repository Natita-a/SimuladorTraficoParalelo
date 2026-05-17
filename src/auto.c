#include <stdio.h>

#include "auto.h"

#define INFINITO 99999

static int calcular_ruta(Ciudad *c, Auto *a) {
    int dist[FILAS][COLUMNAS];
    int vis[FILAS][COLUMNAS];
    Coordenada padre[FILAS][COLUMNAS];

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            dist[i][j] = INFINITO;
            vis[i][j] = 0;
            padre[i][j].x = -1;
            padre[i][j].y = -1;
        }
    }

    dist[a->origen.x][a->origen.y] = 0;

    for (int k = 0; k < FILAS * COLUMNAS; k++) {

        Coordenada u = {-1, -1};
        int menor = INFINITO;

        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                if (!vis[i][j] && dist[i][j] < menor) {
                    menor = dist[i][j];
                    u.x = i;
                    u.y = j;
                }
            }
        }

        if (u.x == -1)
            break;

        if (u.x == a->destino.x && u.y == a->destino.y)
            break;

        vis[u.x][u.y] = 1;

        for (int d = 0; d < 4; d++) {

            int nx = u.x + dx[d];
            int ny = u.y + dy[d];

            if (nx < 0 || nx >= FILAS || ny < 0 || ny >= COLUMNAS)
                continue;

            if (vis[nx][ny])
                continue;

            int peso = 1;

            if (c->grid[nx][ny].tiene_semaforo)
                peso = 2;

            int nd = dist[u.x][u.y] + peso;

            if (nd < dist[nx][ny]) {
                dist[nx][ny] = nd;
                padre[nx][ny] = u;
            }
        }
    }

    if (dist[a->destino.x][a->destino.y] == INFINITO) {
        printf("Auto %d: no se encontro ruta\n", a->id);
        return 0;
    }

    Coordenada tmp[MAX_RUTA];
    int n = 0;
    Coordenada cur = a->destino;

    while (!(cur.x == a->origen.x && cur.y == a->origen.y)) {
        tmp[n++] = cur;
        cur = padre[cur.x][cur.y];
        if (n >= MAX_RUTA)
            break;
    }

    tmp[n++] = a->origen;

    a->tam_ruta = n;

    for (int i = 0; i < n; i++) {
        a->ruta[i] = tmp[n - 1 - i];
    }

    return 1;
}

// CREAR AUTO

void crear_auto(Ciudad *c, Auto *a, int id, Coordenada origen,
                Coordenada destino) {
    a->id = id;
    a->origen = origen;
    a->destino = destino;
    a->idx = 0;
    a->tam_ruta = 0;
    a->activo = 1;

    calcular_ruta(c, a);
}

void actualizar_auto(Ciudad *c, Auto *a) {
    if (!a->activo)
        return;

    if (a->idx >= a->tam_ruta) {
        a->activo = 0;
        printf("Auto %d TERMINO en (%d,%d)\n", a->id, a->destino.x,
               a->destino.y);
        return;
    }

    Coordenada sig = a->ruta[a->idx];

    printf("Auto %d AVANZA a (%d,%d)\n", a->id, sig.x, sig.y);

    a->idx++;
}
